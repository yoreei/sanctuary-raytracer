#pragma once
#include <tuple>
#include <stdexcept>
#include <iterator>
#include <iostream>
#include <source_location>
#include <cuda_runtime.h>
#include <device_launch_parameters.h>

const int SHMEM_SIZE = 256 * 4; // todo unhardcode
const bool CUDA_LOG = false;

struct Color {
    int red;
    int green;
    int blue;
};

std::tuple<size_t, size_t> query_best_kernel_dims(size_t elem_cnt)
{
        size_t threads_cnt = 256;
        size_t blocks_cnt = (elem_cnt + threads_cnt - 1) / threads_cnt;
        return {blocks_cnt, threads_cnt};
}

void chck() {
        cudaError_t status = cudaGetLastError();
        if (status != cudaSuccess) {
                std::string err = cudaGetErrorString(status);
                throw std::runtime_error(err);
        }
        cudaDeviceSynchronize();
        status = cudaGetLastError();
        if (status != cudaSuccess) {
                std::string err = cudaGetErrorString(status);
                throw std::runtime_error(err);
        }
}