
// Sum function credit: Nick from CoffeeBeforeArch
#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "cuda_array.cuh"
#include "common.cuh"

/*
 * Optimization: loop unrolling
 */
static __device__ void d_warp_reduce(int* shmem_ptr, int t)
{
    shmem_ptr[t] += shmem_ptr[t + 32];
        __syncwarp(); // Important: Prevent caching of old values
    shmem_ptr[t] += shmem_ptr[t + 16];
        __syncwarp();
    shmem_ptr[t] += shmem_ptr[t + 8];
         __syncwarp();
    shmem_ptr[t] += shmem_ptr[t + 4];
         __syncwarp();
    shmem_ptr[t] += shmem_ptr[t + 2];
        __syncwarp();
    shmem_ptr[t] += shmem_ptr[t + 1];
}

/* 
 * When max(blockIdx.x) > 1 : v_r[i % blockDim.x == 0] will store blockwise sum
 * When max(blockIdx.x) = 1 : v_r[0] will store the sum of all elements in v
 */
static __global__ void d_cuda_reduce(const cuda_array<int> v,
                                cuda_array<int> v_r, int N)
{
        __shared__ int partial_sum[SHMEM_SIZE];
        int tid = (blockIdx.x * blockDim.x) + threadIdx.x;
        if (tid >= N) {
                printf("stopped thread %d\n", tid);
                return;
        }
        // Optimization: Load elements AND do first add of reduction
        int i = blockIdx.x * (blockDim.x * 2) + threadIdx.x;
        partial_sum[threadIdx.x] = v.d_ptr[i] + v.d_ptr[i + blockDim.x];
        __syncthreads();

        // Optimization: Stop early (finish off with warp_reduce)
        for (int s = blockDim.x / 2; s > 32; s >>= 1) {
                if (threadIdx.x < s) {
                    partial_sum[threadIdx.x] += partial_sum[threadIdx.x + s];
                }
                __syncthreads();
        }

        if (threadIdx.x < 32) {
                d_warp_reduce(partial_sum, threadIdx.x);
        }

        // if 1st pass <num_blocks, num_threads>: store partial sum for each block
        // if 2nd pass <1, num_threads>: store final sum
        if (threadIdx.x == 0) {
                v_r.d_ptr[blockIdx.x] = partial_sum[0];
        }
}

int cuda_reduce(cuda_array<int>::ptr_type& in)
{
        auto [blocks_cnt, threads_cnt] = query_best_kernel_dims(in->size());
        // Optimization: sum reduction requires 2 times less blocks
        blocks_cnt = std::max<size_t>(1, blocks_cnt / 2);
        auto out = cuda_array<int>::make_ptr(threads_cnt, 0);
        printf ("sum_reduction<<<%d, %d>>> processing %d elements",
                blocks_cnt, threads_cnt, in->size()); // TODO use fmt log

        d_cuda_reduce<<<blocks_cnt, threads_cnt>>> (
        *in, *out, in->size()); // TODO refactor max_x*max_y
        chck();
        d_cuda_reduce<<<1, threads_cnt>>> (*out, *out, in->size());
        chck();

        out->cudaMemcpyDeviceToHost();
        return out->h_ptr[0];
}