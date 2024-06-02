// Not finished

void test_cuda_array()
{
        auto arr = model::cuda_array<int>::make_ptr({1, 2, 3});
        assert(arr->size() == 3);
        assert(arr->h_ptr[0] == 1);
        assert(arr->h_ptr[1] == 2);
        assert(arr->h_ptr[2] == 3);
        arr->h_ptr[0] = 4;
        arr->cudaMemcpyHostToDevice();
        arr->cudaMemcpyDeviceToHost();
        assert(arr->h_ptr[0] == 4);
}

void test_compute_map(){

}

void test_compute_reduce(){

}