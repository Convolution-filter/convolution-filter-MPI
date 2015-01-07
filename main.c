#include <stdio.h>
#include <stdlib.h>
#include "functions.h"
include <mpi.h>

int main(void)
{
    int filter[10] = { 9, 9, 9, 9, 9, 9, 9, 9, 9, 9 };
    int * ar = create_random_array(10);
    print_array(ar,10);
    int* dest = malloc(100*sizeof(int));
    compute_internal_values(ar,dest,10,filter);
    putchar('\n');
    print_array(dest,10);



    return 0;
}


/*
 * My first OpenMPI program
 */
int test(int argc, char** argv) {
    int numprocs, rank, namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Get_processor_name(processor_name, &namelen);

    printf("Process %d on %s out of %d\n", rank, processor_name, numprocs);

    MPI_Finalize();
}