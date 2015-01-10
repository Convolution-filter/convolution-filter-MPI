#include <stdio.h>
#include <mpi.h>
#include "hello_mpi.h"


int hello_mpi(void) {
    int numprocs, rank, namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Get_processor_name(processor_name, &namelen);

//    printf("Process %d on %s out of %d\n", rank, processor_name, numprocs);
    if (rank == 0) {
        printf("I'm master\n");
        int slaves_rank = -1;
        int i;
        struct MPI_Status status;
        for (i = 1; i < numprocs; i++) {
            MPI_Recv(&slaves_rank, 1, MPI_INT, i,10111, MPI_COMM_WORLD, &status);
            printf("Master got reply from %d and status is: %d\n", slaves_rank,
                status.MPI_ERROR);
        }
    }
    else {
        printf("Only a slave\n");
        MPI_Send((void*)&rank, 1, MPI_INT, 0, 10111, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
