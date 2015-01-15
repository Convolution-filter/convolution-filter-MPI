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

/* Main arguments:
 * 1. the image to be processed path/<filename>
 * 2. the width of the image
 * 3. the height of the image
 * 4. number of times to apply the filter on the image
 */

int main(int argc, char *argv[])
{
    if ( argc != 5 )
    {
        printf("wrong number of parameters given.\nexiting.");
        return 1;
    }
    char* filename = argv[1];
    int img_width = atoi(argv[2]);
    int img_height = atoi(argv[3]);
    int filter_rounds = atoi(argv[4]);
    int numprocs, rank;

    // Read image and send it
    int *block = initalization_phase(filename, img_width, img_height, 1);


    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int block_width = img_width / sqrt(numprocs) + 2;
    int block_height = img_height / sqrt(numprocs) + 2;

    // Create new communicator (of Cartesian topology)
    int dims[2] = {(int) sqrt(numprocs), (int) sqrt(numprocs)};
    int cyclic[2] = {1, 1};
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, cyclic, 1, &CARTESIAN_COMM);

    int coords[2];
    memset(coords, '\0', sizeof(coords));
    if (CARTESIAN_COMM != MPI_COMM_NULL) {
        MPI_Cart_coords(CARTESIAN_COMM, rank, 2, coords);
        MPI_Cart_rank(CARTESIAN_COMM, coords, &rank);
    }
    else {
        printf("Could not properly set CARTESIAN_COMM_WORLD\n");
        MPI_Finalize();
        return EXIT_FAILURE;
    }
    sleep(2);
    MPI_Barrier(CARTESIAN_COMM);

    block = process_img(block, block_width, block_height, filter_rounds);
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

        int i;
        for (i = 0; i < img_height * img_width; i++) {
            img_buffer[i] = (unsigned char) image[i];
        }
        fwrite(img_buffer, sizeof(char), img_height * img_width, output);
        fclose(output);
        free(img_buffer);
        free(image);
    }
    MPI_Finalize();
    return 0;
}
