#include <iostream>
#include <fstream>

#include "view.h"
#include "cuda_array.cuh"
#include "cuda_map.cuh"

map_data read_data(){
        // TODO read JSON
        map_data md = {
            .max_x = 1024,
            .max_y = 640
        };
        return md;
}

int main() {
        printf("BEGINNING EXECUTION\n");
        { // CUDA device scope
                map_data md = read_data();
                view_data vd {};
                vd.max_x = md.max_x;
                vd.max_y = md.max_y;
                vd.buffer = cuda_map(md);
                
                // vd.print_ppm(std::cout); // DEBUG
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