#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include "functions.h"
#include "send_wrappers.h"
#include "recv_wrappers.h"
#include "hello_mpi.h"
#include "initializations.h"

MPI_Comm CARTESIAN_COMM;

int main()
{
    int numprocs, rank, namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int block_width = 15;
    int block_heigth = 30;
    int filter[9] = { 1, 2, 1, 2, 4, 2, 1, 2, 1 };
    
//    int *ar = create_random_array(block_width, block_heigth);
//    print_array(ar, block_width, block_heigth);
//    int* dest = malloc(block_width * block_heigth * sizeof(int));
//    compute_internal_values(ar,dest, block_width, block_heigth, filter);
//    compute_outer_values(ar, dest, block_width, block_heigth, filter);
//    putchar('\n');
//    print_array(dest, block_width, block_heigth);
    
//    MPI_Init(NULL, NULL);
//    MPI_Get_processor_name(processor_name, &namelen);
    
    // Read image and send it
    int *block = initalization_phase("waterfall_1920_2520.raw", 1920, 2520, 1);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank != 0) {
        sleep(rank + rank);
        printf("I'm slave(%d)", rank);
        print_array(block, 1920/numprocs, 2520/numprocs);
    }
    
    // Create new communicator (of Cartesian topology)
//    printf("One dim: %d\n", ((int) sqrt(numprocs)));
//    int dims[2] = {(int) sqrt(numprocs), (int) sqrt(numprocs)};
//    int cyclic[2] = {1, 1};
//    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, cyclic, 1, &CARTESIAN_COMM);
    
//    int coords[2];
//    memset(coords, '\0', sizeof(coords));
//    if (CARTESIAN_COMM != MPI_COMM_NULL) {
//        MPI_Cart_coords(CARTESIAN_COMM, rank, 2, coords);
//        MPI_Cart_rank(CARTESIAN_COMM, coords, &rank);
//        printf("%8d %8d %8d\n", coords[0], coords[1], rank);
//    }
    
//    MPI_Barrier(MPI_COMM_WORLD);
//    int* dest = malloc(block_width * block_heigth * sizeof(int));
//    compute_internal_values(ar,dest, block_width, block_heigth,filter);
//    compute_outer_values(ar, dest, block_width, block_heigth, filter);
//    putchar('\n');
//    print_array(dest,block_width, block_heigth);
    
    // Create datatype for columns
//    MPI_Datatype mpi_column, mpi_row;
//    MPI_Type_vector(block_heigth - 2, 1, block_width, MPI_INT, &mpi_column);
//    MPI_Type_vector(1, block_width - 2, 0, MPI_INT, &mpi_row);  
//    MPI_Type_commit(&mpi_column);
//    MPI_Type_commit(&mpi_row);
//    if (rank == 0) {
//        printf("I'm master\n");
//        int i;
//        int *ar = create_random_array(block_width, block_heigth);
//        print_array(ar, block_width, block_heigth);
//        int buffer[block_width * block_heigth];
//        memset(buffer, -1, sizeof(buffer));
//        for (i = 1; i < numprocs; i++) {
//            MPI_Request* requests = recv_data(buffer, mpi_column, mpi_row, block_width, block_heigth,
//                10111, rank);
//            wait_on_recv(requests);
//            printf("Master got reply from rank: %d\n", i);
//            print_array(buffer,  block_width, block_heigth);
//            printf("\n----------------------------\n");
//            putchar('\n');
//        }
//    }
//    else {
//        int *ar = create_random_array(block_width, block_heigth);
////        printf("I'm slave (%2d)\n", rank);
//        send_data(ar, mpi_column, mpi_row, rank, numprocs, block_width, block_heigth);
//    }

    MPI_Finalize();
    return 0;
}
