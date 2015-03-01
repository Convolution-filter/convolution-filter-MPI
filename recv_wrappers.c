#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "recv_wrappers.h"
#include "send_wrappers.h"

// Forward declarations
MPI_Request* recv_corner_pixels(int *buffer, int width, int height,
    int my_rank, int current_row, int current_col);

extern MPI_Comm CARTESIAN_COMM;
extern MPI_Datatype mpi_column, mpi_row;

MPI_Request recv_first_row(int* buffer, MPI_Datatype mpi_row, int recv_rank,
    int msg_tag) {
    MPI_Request request;
    MPI_Irecv(buffer + 1, 1, mpi_row, recv_rank, msg_tag, CARTESIAN_COMM, &request);
    return request;
}


MPI_Request recv_last_row(int* buffer, MPI_Datatype mpi_row,
    int width, int height, int recv_rank, int msg_tag) {
    MPI_Request request;
    MPI_Irecv(buffer + width * height - width + 1,
        1, mpi_row, recv_rank, msg_tag, CARTESIAN_COMM, &request);
    return request;
}


MPI_Request recv_left_col(int* buffer, MPI_Datatype mpi_column,
    int width, int recv_rank, int msg_tag) {
    MPI_Request request;
    MPI_Irecv(buffer + width, 1, mpi_column, recv_rank, msg_tag, CARTESIAN_COMM, &request);
    return request;
}


MPI_Request recv_right_col(int* buffer, MPI_Datatype mpi_column,
    int width, int recv_rank, int msg_tag) {
    MPI_Request request;
    MPI_Irecv(buffer + 2 * width - 1, 1, mpi_column, recv_rank, msg_tag, CARTESIAN_COMM, &request);
    return request;
}


MPI_Request* recv_corner_pixels(int *buffer, int width, int height,
    int my_rank, int current_row, int current_col) {
    // Upper left
    int rank_upper_left;
    int coords[] = {current_row - 1, current_col - 1};
    MPI_Cart_rank(CARTESIAN_COMM, coords, &rank_upper_left);
    // Upper right
    int rank_upper_right;
    coords[0] = current_row - 1;
    coords[1] = current_col + 1;
    MPI_Cart_rank(CARTESIAN_COMM, coords, &rank_upper_right);
    // Lower right
    int rank_lower_right;
    coords[0] = current_row + 1;
    coords[1] = current_col + 1;
    MPI_Cart_rank(CARTESIAN_COMM, coords, &rank_lower_right);
    // Lower left
    int rank_lower_left;
    coords[0] = current_row + 1;
    coords[1] = current_col - 1;
    MPI_Cart_rank(CARTESIAN_COMM, coords, &rank_lower_left);

    MPI_Request* requests = malloc (4 * sizeof(MPI_Request));
    if (requests == NULL) {
        fprintf(stderr, "recv_corner_pixel - rank(%3d): Could not allocate "
            "requests array\n", my_rank);
        return NULL;
    }
    MPI_Irecv(buffer, 1, MPI_INT, rank_upper_left, upper_left,
        CARTESIAN_COMM, &(requests[0]));
    MPI_Irecv(buffer + width - 1, 1, MPI_INT, rank_upper_right, upper_right,
        CARTESIAN_COMM, &(requests[1]));
    MPI_Irecv(buffer + width * height - 1, 1, MPI_INT, rank_lower_right,
        lower_right, CARTESIAN_COMM, &(requests[2]));
    MPI_Irecv(buffer + width * (height - 1), 1, MPI_INT, rank_lower_left,
        lower_left, CARTESIAN_COMM, &(requests[3]));
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
MPI_Request* recv_data(int* buffer, int my_rank, int proc_num, int width, 
                        int height) {
    int proc_per_row = sqrt(proc_num);
    int current_row = my_rank / proc_per_row;
    int current_col = my_rank - current_row * proc_per_row;
    // Compute ranks
    // Upper
    int rank_upper;
    int coords[] = {current_row - 1, current_col};
    MPI_Cart_rank(CARTESIAN_COMM, coords, &rank_upper);
    // Right
    int rank_right;
    coords[0] = current_row;
    coords[1] = current_col + 1;
    MPI_Cart_rank(CARTESIAN_COMM, coords, &rank_right);
    // Lower
    int rank_lower;
    coords[0] = current_row + 1;
    coords[1] = current_col;
    MPI_Cart_rank(CARTESIAN_COMM, coords, &rank_lower);
    // Left
    int rank_left;
    coords[0] = current_row;
    coords[1] = current_col - 1;
    MPI_Cart_rank(CARTESIAN_COMM, coords, &rank_left);

    MPI_Request* requests = malloc(8 * sizeof(MPI_Request));
    if (requests == NULL) {
        fprintf(stderr, "recv_data - rank(%3d): Could not allocate requests "
            "array\n", my_rank);
        return NULL;
    }
    memset(requests, '\0', 8 * sizeof(MPI_Request));

    int req_idx = 0;
    assign_requests(requests, &req_idx,
        recv_first_row(buffer, mpi_row, rank_upper, lower));
    assign_requests(requests, &req_idx,
        recv_last_row(buffer, mpi_row, width, height, rank_lower, upper));
    assign_requests(requests, &req_idx,
        recv_left_col(buffer, mpi_column, width, rank_left, right));
    assign_requests(requests, &req_idx,
        recv_right_col(buffer, mpi_column, width, rank_right, left));
    
    MPI_Request* requests_corner_pixels = 
        recv_corner_pixels(buffer, width, height, my_rank, current_row, 
        current_col);
    memcpy(requests + 4, 
        requests_corner_pixels, 4 * sizeof(MPI_Request));
    free(requests_corner_pixels);
    requests_corner_pixels = NULL;
    return requests;
}

void wait_on_recv(MPI_Request* requests) {
    MPI_Status* statuses = malloc(8 * sizeof(MPI_Status));
    int wait_all_retval = MPI_Waitall(8, requests, statuses);
    if (wait_all_retval != MPI_SUCCESS) {
        switch(wait_all_retval) {
            case (MPI_ERR_REQUEST):
                fprintf(stderr, "wait_on_recv: MPI_ERR_REQUEST\n");
                MPI_Finalize();
                exit(EXIT_FAILURE);
            case (MPI_ERR_ARG):
                fprintf(stderr, "wait_on_recv: MPI_ERR_ARG\n");
                MPI_Finalize();
                exit(EXIT_FAILURE);
            case (MPI_ERR_IN_STATUS):
                fprintf(stderr, "wait_on_recv: MPI_ERR_IN_STATUS\n");
                MPI_Finalize();
                exit(EXIT_FAILURE);
            default:
                MPI_Finalize();
                exit(EXIT_FAILURE);
        }
    }
}

