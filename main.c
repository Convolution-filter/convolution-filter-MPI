#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include "processing_img.h"
#include "send_wrappers.h"
#include "recv_wrappers.h"
#include "hello_mpi.h"
#include "initializations.h"

MPI_Comm CARTESIAN_COMM;
extern MPI_Datatype mpi_block, mpi_block_img;

int main()
{
    char filename[] = "waterfall_grey_1920_2520.raw";
    int img_width = 1920;
    int img_height = 2520;
    int numprocs, rank;

    // Read image and send it
    int *block = initalization_phase(filename, img_width, img_height, 1);


    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int block_width = img_width / sqrt(numprocs) + 2;
    int block_height = img_height / sqrt(numprocs) + 2;
    // ---------------------------
    // DEBUG PURPOSES
//    int i;
//    for (i = 0; i < block_width * block_height; i++) {
//        block[i] = rank;
//    }
    // ---------------------------

    // Create new communicator (of Cartesian topology)
//    printf("One dim: %d\n", ((int) sqrt(numprocs)));
    int dims[2] = {(int) sqrt(numprocs), (int) sqrt(numprocs)};
    int cyclic[2] = {1, 1};
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, cyclic, 1, &CARTESIAN_COMM);

    int coords[2];
    memset(coords, '\0', sizeof(coords));
    if (CARTESIAN_COMM != MPI_COMM_NULL) {
        MPI_Cart_coords(CARTESIAN_COMM, rank, 2, coords);
        MPI_Cart_rank(CARTESIAN_COMM, coords, &rank);
//        printf("%8d %8d %8d\n", coords[0], coords[1], rank);
    }
    else {
        printf("Could not properly set CARTESIAN_COMM_WORLD\n");
        MPI_Finalize();
        return EXIT_FAILURE;
    }
    sleep(2);
    MPI_Barrier(CARTESIAN_COMM);

    block = process_img(block, block_width, block_height, 2);
    int* image = NULL;
    if (rank != 0) {
        MPI_Gather(block + block_width + 1, 1, mpi_block, image, 1, mpi_block_img, 0, CARTESIAN_COMM);
    }
    else {
        image = malloc(img_width * img_height * sizeof(int));
        MPI_Gather(block + block_width + 1, 1, mpi_block, image, 1, mpi_block_img, 0, CARTESIAN_COMM);
        // write BW image as raw file
        FILE* output = fopen("output.raw", "wb");
        unsigned char* img_buffer = malloc(img_height * img_width *
                                        sizeof(unsigned char));
        sleep(4);
        //print_array(image, img_width, img_height);
//        sleep(2);
        int i;
        for (i = 0; i < img_height * img_width; i++) {
            img_buffer[i] = (unsigned char) image[i];
            //printf("%4u ", img_buffer[i]);
            //if ((i != 0) && ((i + 1) % img_width == 0))
             //   putchar('\n');
        }
        putchar('\n');
        fwrite(img_buffer, sizeof(char), img_height * img_width, output);
        fclose(output);
        free(img_buffer);
        free(image);
    }
    MPI_Finalize();
    return 0;
}
