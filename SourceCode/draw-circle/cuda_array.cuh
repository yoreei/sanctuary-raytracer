#pragma once
#include <memory>
#include "common.cuh"
#include <cuda_runtime.h>

/*
Wrapper around device & host memory representing the same object.
*/
template<typename T>
struct cuda_array {
public:
        T* d_ptr = nullptr;
        T* h_ptr = nullptr;

private:
        size_t _size = 0;

public:
        __device__ __host__ size_t size() const { return _size; }

        struct deleter {
        void operator()(cuda_array* obj) const
        {
                if (obj) {
                cudaFree(obj->d_ptr);
                chck();
                cudaFreeHost(obj->h_ptr);
                chck();
                }
        }
        };

        using ptr_type = std::unique_ptr<cuda_array<T>, cuda_array<T>::deleter>;

        template<typename... Args>
        static ptr_type make_ptr(Args&&... args) {
                auto* ptr = new cuda_array<T>(std::forward<Args>(args)...);
                return ptr_type(ptr, cuda_array<T>::deleter{});
        }

        static ptr_type make_ptr(std::initializer_list<T> init) {
                auto* ptr = new cuda_array<T>(init);
                return ptr_type(ptr, cuda_array<T>::deleter{});
        }

        cuda_array(size_t num_elements, T fill) : cuda_array(num_elements) {
                std::fill_n(h_ptr, num_elements, fill);
                cudaMemcpy(d_ptr, h_ptr, size_bytes(), ::cudaMemcpyHostToDevice);
                chck();
        }

        cuda_array(std::initializer_list<T> init) : cuda_array(init.size()) {
                std::copy(init.begin(), init.end(), h_ptr);
                cudaMemcpy(d_ptr, h_ptr, size_bytes(), ::cudaMemcpyHostToDevice);
                chck();
        }

        explicit cuda_array(size_t num_elements) : _size(num_elements) {
                cudaError_t status = cudaMalloc(&d_ptr, size_bytes());
                chck();
                status = cudaMallocHost(&h_ptr, size_bytes());
                chck();
        }

        /* TODO: Initialize cuda_array with a C array, taking ownership */

        using iterator = T*;
        using const_iterator = const T*;

        iterator begin() noexcept {
                return h_ptr;
        }

        iterator end() noexcept {
                return h_ptr + _size;
        }

        const_iterator begin() const noexcept {
                return h_ptr;
        }

        const_iterator end() const noexcept {
                return h_ptr + _size;
        }

        const_iterator cbegin() const noexcept {
                return h_ptr;
        }

        const_iterator cend() const noexcept {
                return h_ptr + _size;
        }

        void cudaMemcpyHostToDevice()
        {
                cudaMemcpy(d_ptr, h_ptr, size_bytes(), ::cudaMemcpyHostToDevice);
                chck();
        }

        void cudaMemcpyDeviceToHost()
        {
                cudaMemcpy(h_ptr, d_ptr, size_bytes(), ::cudaMemcpyDeviceToHost);
                chck();
        }

private:
        size_t size_bytes() const { return _size * sizeof(T); }
};
