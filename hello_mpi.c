#include <stdio.h>
#include <mpi.h>
#include <unistd.h>
#include "hello_mpi.h"


int hello_mpi(void) {
    int numprocs, rank, namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Get_processor_name(processor_name, &namelen);

    if (rank == 0) {
        printf("I'm master\n");
        int slaves_rank = -1;
        int i;
        MPI_Status status;
        MPI_Request request;
        for (i = 1; i < numprocs; i++) {
            MPI_Irecv(&slaves_rank, 1, MPI_INT, i, 10111, MPI_COMM_WORLD, &request);
            MPI_Wait(&request, &status);
            printf("Master got reply from %d and request is: %d\n", slaves_rank,
                request);
        }
    }
    else {
        MPI_Request request;
        MPI_Isend((void*)&rank, 1, MPI_INT, 0, 10111, MPI_COMM_WORLD, &request);
        printf("Only a slave - with request %d\n", request);
    }

    MPI_Finalize();
    return 0;
}
