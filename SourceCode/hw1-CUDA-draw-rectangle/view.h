#pragma once
#include <numeric>
#include "cuda_array.cuh"
#include "common.cuh"

class view_data {
public:
        cuda_array<Color>::ptr_type buffer;
        int max_x;
        int max_y;
public:
    void print_ppm(std::ostream& out)
    {
        // PPM header
        out << "P3" << std::endl;
        out << max_x << " " << max_y << std::endl;
        out << "255" << std::endl;

        size_t x_cnt = 0;

        auto pixels = buffer->h_ptr;
        for (auto idx = 0; idx < max_x * max_y; ++idx) {
            out << pixels[idx].red << " "
                << pixels[idx].green << " "
                << pixels[idx].blue << " ";
            ++x_cnt;

            if (x_cnt == max_x) {
                out << std::endl;
                x_cnt = 0;
            }
        }
    }
};
