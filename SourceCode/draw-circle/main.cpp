#include <iostream>
#include <fstream>

#include "view.h"
#include "cuda_array.cuh"
#include "cuda_map.cuh"

map_data read_data(){
        // TODO read JSON
        // 
        // tower: circle
        // poi: rectangle, which will intersect the circle. Think of this a that part of the canvas we are allowed to draw on
        map_data md = {
            .tower_radius = cuda_array<int>::make_ptr({32}),
            .tower_x = cuda_array<int>::make_ptr({32}),
            .tower_y = cuda_array<int>::make_ptr({32}),
            .poi_x = cuda_array<int>::make_ptr({0,64}), // begin, end
            .poi_y = cuda_array<int>::make_ptr({0,64}), // begin, end
            .max_x = 64,
            .max_y = 64
        };
        return md;
}

int main() {
        printf("BEGINNING EXECUTION\n");
        // Intersects 5g tower radius with points of interest. Geometrically, intersects circles (towers) with squares (poi's)
        { // CUDA device scope
                map_data md = read_data();
                view_data vd {};
                vd.max_x = md.max_x;
                vd.max_y = md.max_y;
                vd.buffer = cuda_map(md);
                
                vd.print_ppm(std::cout);
                std::ofstream outfile("output.ppm");
                if (outfile.is_open()) {
                    vd.print_ppm(outfile);
                    outfile.close();
                } else {
                    std::cerr << "Failed to open file for writing: output.ppm" << std::endl;
                }
                //show_window(); // show directX

        } // CUDA device scope

        cudaDeviceReset();
        return 0;
}