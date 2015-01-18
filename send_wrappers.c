#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <string.h>
#include "send_wrappers.h"
#include "processing_img.h"

// Forward declarations
MPI_Request send_left_column(int* src, int width, MPI_Datatype mpi_column,
    int receiver_rank, int msg_tag);
MPI_Request send_right_column(int* src, int width, MPI_Datatype mpi_column,
    int receiver_rank, int msg_tag);
MPI_Request send_first_row(int* src, int width, MPI_Datatype mpi_row,
    int receiver_rank, int msg_tag);
MPI_Request send_last_row(int* src, int width, int height, MPI_Datatype mpi_row,
    int receiver_rank, int msg_tag);

extern MPI_Comm CARTESIAN_COMM;
extern MPI_Datatype mpi_column, mpi_row;

// ---------------------------------------------
// Directions handler
// ---------------------------------------------
/*
 * Input:
 *  my_rank :   current process rank
 *  proc_num :  total number of processes
 *  direction : where to send it
 * Return value:
 *  The process rank to which we need to send the data
 */
//int get_proc_idx(int my_rank, int proc_num, int target_direction ){
//    if (target_direction == direction.up_left) {
//        // send
//    }
//}

// ---------------------------------------------
// Sending columns and rows wrappers
// ---------------------------------------------
MPI_Request send_left_column(int* src, int width, MPI_Datatype mpi_column,
    int receiver_rank, int msg_tag) {
    MPI_Request request;
    MPI_Isend(src + width + 1, 1, mpi_column, receiver_rank, msg_tag,
        CARTESIAN_COMM, &request);
    return request;
}

MPI_Request send_right_column(int* src, int width, MPI_Datatype mpi_column,
    int receiver_rank, int msg_tag) {
    MPI_Request request;
    MPI_Isend(src + 2 * width - 2, 1, mpi_column, receiver_rank, msg_tag,
        CARTESIAN_COMM, &request);
    return request;
}

MPI_Request send_first_row(int* src, int width, MPI_Datatype mpi_row,
    int receiver_rank, int msg_tag) {
    MPI_Request request;
    MPI_Isend(src + width + 1, 1, mpi_row, receiver_rank, msg_tag, CARTESIAN_COMM,
        &request);
    return request;
}

MPI_Request send_last_row(int* src, int width, int height, MPI_Datatype mpi_row,
                        int receiver_rank, int msg_tag) {
    MPI_Request request;
    MPI_Isend(src + width * (height - 1) - width + 1, 1, mpi_row, receiver_rank,
        msg_tag, CARTESIAN_COMM, &request);
    return request;
}

MPI_Request send_corner_pixel(int* src, int width, int height,
    int receiver_rank, direction given_corner) {
    MPI_Request request;
    switch (given_corner) {
        case upper_left:
            MPI_Isend(src + width + 1, 1, MPI_INT, receiver_rank,
                lower_right, CARTESIAN_COMM, &request);
            break;
        case upper_right:
            MPI_Isend(src + 2 * width - 2, 1, MPI_INT, receiver_rank,
                lower_left,
                CARTESIAN_COMM, &request);
            break;
        case lower_right:
            MPI_Isend(src + width * (height - 1) - 2, 1, MPI_INT,
                receiver_rank, upper_left, CARTESIAN_COMM, &request);
            break;
        case lower_left:
            MPI_Isend(src + width * (height - 1) - width + 1, 1, MPI_INT,
                receiver_rank, upper_right, CARTESIAN_COMM, &request);
            break;
        default:
            fprintf(stderr, "send_corner_pixel: This shouldn't be printed at all"
                " ever! There's sth really wrong\n");
            break;
    }
    return request;
}

MPI_Request* send_data(int* src, int my_rank, int proc_num, 
                        int width, int height) {
    int proc_per_row = sqrt(proc_num);
    int current_row = my_rank / proc_per_row;
    int current_col = my_rank - current_row * proc_per_row;
    // Compute ranks
    // Upper left
    int rank_upper_left;
    int coords[] = {current_row - 1, current_col - 1};
    MPI_Cart_rank(CARTESIAN_COMM, coords, &rank_upper_left);
    // Upper
    int rank_upper;
    coords[0] = current_row - 1;
    coords[1] = current_col;
    MPI_Cart_rank(CARTESIAN_COMM, coords, &rank_upper);
    // Upper right
    int rank_upper_right;
    coords[0] = current_row - 1;
    coords[1] = current_col + 1;
    MPI_Cart_rank(CARTESIAN_COMM, coords, &rank_upper_right);
    // Right
    int rank_right;
    coords[0] = current_row;
    coords[1] = current_col + 1;
    MPI_Cart_rank(CARTESIAN_COMM, coords, &rank_right);
    // Lower right
    int rank_lower_right;
    coords[0] = current_row + 1;
    coords[1] = current_col + 1;
    MPI_Cart_rank(CARTESIAN_COMM, coords, &rank_lower_right);
    // Lower
    int rank_lower;
    coords[0] = current_row + 1;
    coords[1] = current_col;
    MPI_Cart_rank(CARTESIAN_COMM, coords, &rank_lower);
    // Lower left
    int rank_lower_left;
    coords[0] = current_row + 1;
    coords[1] = current_col - 1;
    MPI_Cart_rank(CARTESIAN_COMM, coords, &rank_lower_left);
    // Left
    int rank_left;
    coords[0] = current_row;
    coords[1] = current_col - 1;
    MPI_Cart_rank(CARTESIAN_COMM, coords, &rank_left);

    MPI_Request *requests = malloc(8 * sizeof(MPI_Request));
    memset(requests, '\0', 8 * sizeof(MPI_Request));
    int req_idx = 0;
    assign_requests(requests, &req_idx,
    	send_first_row(src, width, mpi_row, rank_upper , upper));
    assign_requests(requests, &req_idx,
    	send_last_row(src, width, height, mpi_row, rank_lower , lower));
    assign_requests(requests, &req_idx,
    	send_left_column(src, width, mpi_column, rank_left, left));
    assign_requests(requests, &req_idx,
    	send_right_column(src, width, mpi_column, rank_right, right));
    assign_requests(requests, &req_idx,
    	send_corner_pixel(src, width, height, rank_lower_left, lower_left));
    assign_requests(requests, &req_idx,
    	send_corner_pixel(src, width, height, rank_lower_right, lower_right));
    assign_requests(requests, &req_idx,
    	send_corner_pixel(src, width, height, rank_upper_left, upper_left));
    assign_requests(requests, &req_idx,
    	send_corner_pixel(src, width, height, rank_upper_right, upper_right));
    return requests;
}


void assign_requests(MPI_Request* requests, int* idx, MPI_Request req_val) {
    requests[*idx] = req_val;
    (*idx)++;
}


void wait_on_send(MPI_Request* requests) {
    MPI_Status* statuses = malloc(8 * sizeof(MPI_Status));
    int wait_all_retval = MPI_Waitall(8, requests, statuses);
    if (wait_all_retval != MPI_SUCCESS) {
        switch(wait_all_retval) {
            case (MPI_ERR_REQUEST):
                fprintf(stderr, "wait_on_send: MPI_ERR_REQUEST\n");
                MPI_Finalize();
                exit(EXIT_FAILURE);
            case (MPI_ERR_ARG):
                fprintf(stderr, "wait_on_send: MPI_ERR_ARG\n");
                MPI_Finalize();
                exit(EXIT_FAILURE);
            case (MPI_ERR_IN_STATUS):
                fprintf(stderr, "wait_on_send: MPI_ERR_IN_STATUS\n");
                MPI_Finalize();
                exit(EXIT_FAILURE);
            default:
                MPI_Finalize();
                exit(EXIT_FAILURE);
        }
    }
}
