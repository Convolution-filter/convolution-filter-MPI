#include <stdio.h>
#include <stdlib.h>
#include "functions.h"
//#include <mpi.h>

int main(void)
{
    int block_size = 10;
    int filter[9] = { 1, 2, 1, 2, 4, 2, 1, 2, 1 };
    int * ar = create_random_array(block_size);
    print_array(ar,block_size);
    int* dest = malloc(block_size * block_size * sizeof(int));
    compute_internal_values(ar,dest,block_size,filter);
    compute_outer_values(ar, dest, block_size, filter);
    putchar('\n');
    print_array(dest,block_size);
    return 0;
}


/*
 * My first OpenMPI program
 */
//int test(int argc, char** argv) {
//    int numprocs, rank, namelen;
//    char processor_name[MPI_MAX_PROCESSOR_NAME];
//
//    MPI_Init(&argc, &argv);
//    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
//    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
//    MPI_Get_processor_name(processor_name, &namelen);
//
//    printf("Process %d on %s out of %d\n", rank, processor_name, numprocs);
//
//    MPI_Finalize();
//}