#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include "cuda_array.cuh"
#include "common.cuh"

static const int NO_SIGNAL = 0;
static const int HAS_SIGNAL = 1;

struct map_data {
        cuda_array<int>::ptr_type tower_radius;
        cuda_array<int>::ptr_type tower_x;
        cuda_array<int>::ptr_type tower_y;
        cuda_array<int>::ptr_type poi_x;
        cuda_array<int>::ptr_type poi_y;
        int max_x ;
        int max_y;
};

static __global__ void d_cuda_map (
        const cuda_array<int> tower_radius, const cuda_array<int> tower_x,
        const cuda_array<int> tower_y, const cuda_array<int> poi_x, 
        const cuda_array<int> poi_y, const int max_x, const int max_y,
        cuda_array<int> buffer2d
  )
 {
        int tid = (blockIdx.x * blockDim.x) + threadIdx.x;
        if (tid >= max_x * max_y) return;
        int x = tid % max_x;
        int y = tid / max_x;
        buffer2d.d_ptr[tid] = NO_SIGNAL; 
        for (size_t i = 0; i < tower_radius.size(); ++i) {
                //printf("x: %d, y: %d, tower_x: %d, tower_y: %d, tower_radius: %d\n",
                //        x, y, tower_x.d_ptr[i], tower_y.d_ptr[i], tower_radius.d_ptr[i]);
                int dx = x - tower_x.d_ptr[i];
                int dy = y - tower_y.d_ptr[i];
                int d2 = dx * dx + dy * dy; 
                int r2 = tower_radius.d_ptr[i] * tower_radius.d_ptr[i];
                if (d2 <= r2) {
                //printf("INTERSECT: x: %d, y: %d, tower_x: %d, tower_y: %d, tower_radius: %d\n",
                //        x, y, tower_x.d_ptr[i], tower_y.d_ptr[i], tower_radius.d_ptr[i]);
                buffer2d.d_ptr[tid] = HAS_SIGNAL;
                break; 
                }
        }
}

cuda_array<int>::ptr_type cuda_map(map_data& in)
{
        uint32_t max_x = *std::max_element(in.poi_x->begin(), in.poi_x->end());
        uint32_t max_y = *std::max_element(in.poi_y->begin(), in.poi_y->end());
        size_t N = max_x * max_y;
        auto [blocks_cnt, threads_cnt] = query_best_kernel_dims(N);
        auto buffer2d = cuda_array<int>::make_ptr(N, 0); // fill 0
        //printf("compute_map<<<%d, %d>>> processing %d elements",
        //        blocks_cnt, threads_cnt, N); // TODO use fmt log

        d_cuda_map<<<blocks_cnt, threads_cnt>>>(
        *in.tower_radius, *in.tower_x, *in.tower_y, *in.poi_x,
        *in.poi_y, max_x, max_y, *buffer2d);
        chck();

        buffer2d->cudaMemcpyDeviceToHost();
        return buffer2d;
}