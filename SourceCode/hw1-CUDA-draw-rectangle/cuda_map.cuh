#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include "cuda_array.cuh"
#include "common.cuh"

__device__ const Color PALETTE[10] = {
    {52, 152, 219},  // Peter River (Blue)
    {231, 76, 60},   // Alizarin (Red)
    {46, 204, 113},  // Emerald (Green)
    {241, 196, 15},  // Sun Flower (Yellow)
    {155, 89, 182},  // Amethyst (Purple)
    {52, 73, 94},    // Wet Asphalt (Dark Blue)
    {243, 156, 18},  // Orange
    {26, 188, 156},  // Turquoise (Teal)
    {149, 165, 166}, // Concrete (Gray)
    {236, 240, 241}  // Clouds (Light Gray)
};

struct map_data {
        int max_x ;
        int max_y;
};

static __global__ void d_cuda_map (
        const int max_x, const int max_y, cuda_array<Color> buffer2d
  )
 {
        int tid = (blockIdx.x * blockDim.x) + threadIdx.x;
        if (tid >= max_x * max_y) return;
        int x = tid % max_x;
        int y = tid / max_x;
        if (x < max_x / 2 && y < max_y / 2) {
            buffer2d.d_ptr[tid] = PALETTE[0];
        }
        else if (x >= max_x / 2 && y < max_y / 2) {
            buffer2d.d_ptr[tid] = PALETTE[1];
        } else if (x < max_x / 2 && y >= max_y / 2) {
            buffer2d.d_ptr[tid] = PALETTE[2];
        } else {
            buffer2d.d_ptr[tid] = PALETTE[3];
        }

}

cuda_array<Color>::ptr_type cuda_map(map_data& in)
{
        size_t N = in.max_x * in.max_y;
        auto [blocks_cnt, threads_cnt] = query_best_kernel_dims(N);
        auto buffer2d = cuda_array<Color>::make_ptr(N, Color{0,0,0}); // fill 0
        //printf("compute_map<<<%d, %d>>> processing %d elements",
        //        blocks_cnt, threads_cnt, N); // TODO use fmt log

        d_cuda_map<<<blocks_cnt, threads_cnt>>>(
        in.max_x, in.max_y, *buffer2d);
        chck();

        buffer2d->cudaMemcpyDeviceToHost();
        return buffer2d;
}