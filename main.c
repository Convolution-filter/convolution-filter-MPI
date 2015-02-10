#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include "processing_img.h"
#include "send_wrappers.h"
#include "recv_wrappers.h"
#include "initializations.h"

MPI_Comm CARTESIAN_COMM;
extern MPI_Datatype mpi_block, mpi_block_img;

int* process_img_wrapper(int* block, int block_width, int block_height, int filter_rounds, int convergence_option,
                        int convergence_rounds, int rank, int img_width, int img_height);
unsigned char* convert_img(int* image, int img_height, int img_width);

/* Main arguments:
 * 1. the image to be processed path/<filename>
 * 2. the width of the image
 * 3. the height of the image
 * 4. number of times to apply the filter on the image
 * 5. is black and white (0/1)
 * 6. convergence (0/1)
 * 7. number of rounds to check for convergence
 */

int main(int argc, char *argv[])
{

    if ( argc < 7 )
    {
        printf("Wrong number of parameters given.\n");
        printf("Usage: %s <img_filename> <width> <height> <number of repetitions"
            "> <isBW image (0/1)> <withConvergence(0/1)> <number of rounds to "
            "check for convergence [optional]>\n", argv[0]);
        return 1;
    }
    int convergence_option = atoi(argv[6]);
    if ( convergence_option == 1 && argc != 8 )
    {
        printf("Wrong number of parameters given.\n");
        printf("Usage: %s <img_filename> <width> <height> <number of repetitions"
            "> <isBW image (0/1)> <withConvergence(0/1)> <number of rounds to "
            "check for convergence [optional]>\n", argv[0]);
        return 1;
    }
    char* filename = argv[1];
    int img_width = atoi(argv[2]);
    int img_height = atoi(argv[3]);
    int filter_rounds = atoi(argv[4]);
    int isBW = atoi(argv[5]);
    int convergence_rounds = -1;

    if ( convergence_option == 1 )
        convergence_rounds = atoi(argv[7]);

    int numprocs, rank;

    // Read image and send it
    int *block, **rgb_block;
    if ( isBW )
        block = *(initalization_phase(filename, img_width, img_height, 1));
    else
        rgb_block = initalization_phase(filename, img_width, img_height, 0);


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
    double start_time = MPI_Wtime();

    if ( isBW )
    {
        block = process_img(block, block_width, block_height, filter_rounds, convergence_option, convergence_rounds);
        int* image = NULL;
        if (rank != 0) {
            MPI_Gather(block + block_width + 1, 1, mpi_block, image, 1, mpi_block_img, 0, CARTESIAN_COMM);
        }
        else {
            image = malloc(img_width * img_height * sizeof(int));
            MPI_Gather(block + block_width + 1, 1, mpi_block, image, 1, mpi_block_img, 0, CARTESIAN_COMM);
            double end_time = MPI_Wtime();

            printf("%f", end_time - start_time);
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
    }
    else
    {
        int** rgb_img_buffer = malloc( 3 * sizeof(int*) );
        int i;
        for ( i = 0; i < 3; i++)
        {
            rgb_img_buffer[i] = process_img_wrapper(rgb_block[i], block_width, block_height, filter_rounds, convergence_option,
                                                    convergence_rounds, rank, img_width, img_height);
        }
        if (rank == 0)
        {
            int i, j;
            double end_time = MPI_Wtime();
            printf("elapsed seconds: %f\n", end_time - start_time);

            unsigned char** image_buffer = malloc( 3 * sizeof(unsigned char*));
            for ( i = 0; i < 3; i++)
            {
                image_buffer[i] = convert_img(rgb_img_buffer[i], img_height, img_width);
                free(rgb_img_buffer[i]);
            }
            FILE* output = fopen("output.raw", "wb");
            unsigned char* image = malloc( 3 * img_height * img_width * sizeof(unsigned char));
            for ( i = 0 , j = 0; i < img_height * img_width; i++ , j += 3 )
            {
                image[j] = image_buffer[0][i];
                image[j+1] = image_buffer[1][i];
                image[j+2] = image_buffer[2][i];
            }
            fwrite(image, sizeof(char), 3 * img_height * img_width, output);
            fclose(output);
            for ( i = 0; i < 3; i++)
            {
                free(image_buffer[i]);
            }
            free(image_buffer);
            free(image);
        }
        free(rgb_img_buffer);
    }
    MPI_Finalize();
    return 0;
}

int* process_img_wrapper(int* block, int block_width, int block_height, int filter_rounds, int convergence_option,
                        int convergence_rounds, int rank, int img_width, int img_height)
{
    block = process_img(block, block_width, block_height, filter_rounds, convergence_option, convergence_rounds);
    int* image = NULL;
    if (rank != 0) {
        MPI_Gather(block + block_width + 1, 1, mpi_block, image, 1, mpi_block_img, 0, CARTESIAN_COMM);
        return NULL;
    }
    else {
        image = malloc(img_width * img_height * sizeof(int));
        MPI_Gather(block + block_width + 1, 1, mpi_block, image, 1, mpi_block_img, 0, CARTESIAN_COMM);
        return image;
    }
}

unsigned char* convert_img(int* image, int img_height, int img_width)
{
    unsigned char* img_buffer = malloc(img_height * img_width *
                                            sizeof(unsigned char));
    int i;
    for (i = 0; i < img_height * img_width; i++) {
        img_buffer[i] = (unsigned char) image[i];
    }
    return img_buffer;
}
