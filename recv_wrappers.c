#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <string.h>
#include "recv_wrappers.h"
#include "send_wrappers.h"


MPI_Request recv_first_row(int* buffer, MPI_Datatype mpi_row, int recv_rank, 
    int msg_tag) {
    MPI_Request request;
    MPI_Irecv(buffer + 1, 1, mpi_row, recv_rank, msg_tag, MPI_COMM_WORLD, &request);
    return request;
}

MPI_Request recv_last_row(int* buffer, MPI_Datatype mpi_row, 
    int width, int height, int recv_rank, int msg_tag) {
    MPI_Request request;
    MPI_Irecv(buffer + width * height - width + 1, 
        1, mpi_row, recv_rank, msg_tag, MPI_COMM_WORLD, &request);
    return request;
}
    
MPI_Request recv_left_col(int* buffer, MPI_Datatype mpi_column, 
    int width, int recv_rank, int msg_tag) {
    MPI_Request request;
    MPI_Irecv(buffer + width, 1, mpi_column, recv_rank, msg_tag, MPI_COMM_WORLD, &request);
    return request;
}

MPI_Request recv_right_col(int* buffer, MPI_Datatype mpi_column, 
    int width, int recv_rank, int msg_tag) {
    MPI_Request request;
    MPI_Irecv(buffer + 2 * width - 1, 1, mpi_column, recv_rank, msg_tag, MPI_COMM_WORLD, &request);
    return request;
}

MPI_Request* recv_corner_pixels(int *buffer, int width, int height, 
    int my_rank) {
    MPI_Request* requests = malloc (4 * sizeof(MPI_Request));
    if (requests == NULL) {
        fprintf(stderr, "recv_corner_pixel - rank(%3d): Could not allocate "
            "requests array\n", my_rank);
        return NULL;
    }
    int recv_rank = 1;
    MPI_Irecv(buffer, 1, MPI_INT, recv_rank, upper_left, 
        MPI_COMM_WORLD, &(requests[0]));
    MPI_Irecv(buffer + width - 1, 1, MPI_INT, recv_rank, upper_right, 
        MPI_COMM_WORLD, &(requests[1]));
    MPI_Irecv(buffer + width * height - 1, 1, MPI_INT, recv_rank, 
        lower_right, MPI_COMM_WORLD, &(requests[2]));
    MPI_Irecv(buffer + width * (height - 1), 1, MPI_INT, recv_rank, 
        lower_left, MPI_COMM_WORLD, &(requests[3]));
    return requests;
}

/*
 * Receives data
 * Input:
 *  buffer :        The destination array
 *  mpi_column/row: The defined MPI datatypes
 *  width:          The width of the block
 *  height:         The height of the block
 *  msg_tag:        The message tag to be used 
 *  my_rank:        The rank of the current process
 * Return value:
 *  An MPI_Requests array, for use with the MPI_Wait method as handles
 */
MPI_Request* recv_data(int* buffer, MPI_Datatype mpi_column, 
    MPI_Datatype mpi_row, int width, int height, int msg_tag, int my_rank) {
    MPI_Request* requests = malloc(8 * sizeof(MPI_Request));
    if (requests == NULL) {
        fprintf(stderr, "recv_data - rank(%3d): Could not allocate requests "
            "array\n", my_rank);
        return NULL;
    }
    memset(requests, '\0', 8 * sizeof(MPI_Request));
    requests[0] = recv_first_row(buffer, mpi_row, 1, msg_tag);
    requests[1] = recv_last_row(buffer, mpi_row, width, height, 1, msg_tag);
    requests[2] = recv_left_col(buffer, mpi_column, width, 1, msg_tag);
    requests[3] = recv_right_col(buffer, mpi_column, width, 1, msg_tag);
    memcpy(requests + 4, 
        recv_corner_pixels(buffer, width, height, my_rank), 
        4 * sizeof(MPI_Request));
    return requests;
}

void wait_on_recv(MPI_Request* requests) {
    int i;
    for (i = 0; i < 8; i++) {
        MPI_Status status;
        MPI_Wait(&(requests[i]), &status);
        // TODO handle status
    }
}

