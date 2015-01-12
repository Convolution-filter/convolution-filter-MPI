#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>
#include "functions.h"
#include "communication_functions.h"
#include "hello_mpi.h"

int main()
{
    int numprocs, rank, namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int block_width = 15;
    int block_heigth = 10;
    int filter[9] = { 1, 2, 1, 2, 4, 2, 1, 2, 1 };
//    int *ar = create_random_array(block_width, block_heigth);
//    print_array(ar, block_width, block_heigth);
//    int* dest = malloc(block_width * block_heigth * sizeof(int));
//    compute_internal_values(ar,dest, block_width, block_heigth, filter);
//    compute_outer_values(ar, dest, block_width, block_heigth, filter);
//    putchar('\n');
//    print_array(dest, block_width, block_heigth);
    
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Get_processor_name(processor_name, &namelen);
    
    
//    int* dest = malloc(block_width * block_heigth * sizeof(int));
//    compute_internal_values(ar,dest, block_width, block_heigth,filter);
//    compute_outer_values(ar, dest, block_width, block_heigth, filter);
//    putchar('\n');
//    print_array(dest,block_width, block_heigth);
    
    // Create datatype for columns
    MPI_Datatype mpi_column, mpi_row;
    MPI_Type_vector(block_heigth - 2, 1, block_width, MPI_INT, &mpi_column);
    MPI_Type_vector(1, block_width - 2, 0, MPI_INT, &mpi_row);
    MPI_Type_commit(&mpi_column);
    MPI_Type_commit(&mpi_row);
    if (rank == 0) {
        printf("I'm master\n");
        int i;
        int *ar = create_random_array(block_width, block_heigth);
        print_array(ar, block_width, block_heigth);
        MPI_Status status;
        MPI_Request request;
        int buffer[block_width * block_heigth];
        memset(buffer, -1, sizeof(buffer));
        for (i = 1; i < numprocs; i++) {
            MPI_Irecv(buffer + block_width, 1, mpi_column, i, 10111, MPI_COMM_WORLD, &request);
            MPI_Wait(&request, &status);
            MPI_Irecv(buffer + 2 * block_width - 1, 1, mpi_column, i, 10111, MPI_COMM_WORLD, &request);
            MPI_Wait(&request, &status);
            MPI_Irecv(buffer + 1, 1, mpi_row, i, 10111, MPI_COMM_WORLD, &request);
            MPI_Wait(&request, &status);
            MPI_Irecv(buffer + block_width * block_heigth - block_width + 1, 1, mpi_row, i, 10111, MPI_COMM_WORLD, &request);
            MPI_Wait(&request, &status);
            printf("Master got reply from rank: %d\n", i);
            print_array(buffer,  block_width, block_heigth);
            printf("\n----------------------------\n");
            putchar('\n');
        }
    }
    else {
        int *ar = create_random_array(block_width, block_heigth);
        send_data(ar, mpi_column, mpi_row, rank, numprocs, block_width, block_heigth);
    }

    MPI_Finalize();
    return 0;
}
