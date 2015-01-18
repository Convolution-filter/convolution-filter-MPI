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

int main()
{
    char filename[] = "waterfall_1920_2520.raw";
    int img_width = 20;
    int img_height = 40;
    int numprocs, rank;
    int filter[9] = { 1, 2, 1, 2, 4, 2, 1, 2, 1 };
    
    // Read image and send it
    int *block = initalization_phase(filename, img_width, img_height, 1);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    int block_width = img_width / sqrt(numprocs) + 2;
    int block_height = img_height / sqrt(numprocs) + 2;
    
    // Create new communicator (of Cartesian topology)
    printf("One dim: %d\n", ((int) sqrt(numprocs)));
    int dims[2] = {(int) sqrt(numprocs), (int) sqrt(numprocs)};
    int cyclic[2] = {1, 1};
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, cyclic, 1, &CARTESIAN_COMM);
    
    int coords[2];
    memset(coords, '\0', sizeof(coords));
    if (CARTESIAN_COMM != MPI_COMM_NULL) {
        MPI_Cart_coords(CARTESIAN_COMM, rank, 2, coords);
        MPI_Cart_rank(CARTESIAN_COMM, coords, &rank);
        printf("%8d %8d %8d\n", coords[0], coords[1], rank);
    }
    else {
        printf("Could not properly set CARTESIAN_COMM_WORLD\n");
        MPI_Finalize();
        return EXIT_FAILURE;
    }
    
    MPI_Barrier(MPI_COMM_WORLD);
//    compute_internal_values(ar,dest, block_width, block_heigth,filter);
//    compute_outer_values(ar, dest, block_width, block_heigth, filter);
    
    MPI_Finalize();
    return 0;
}
