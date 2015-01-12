#include <mpi.h>
#include <math.h>
#include "send_wrappers.h"

// Forward declarations
MPI_Request send_left_column(int* src, int width, MPI_Datatype mpi_column, 
    int receiver_rank, int msg_tag);
MPI_Request send_right_column(int* src, int width, MPI_Datatype mpi_column, 
    int receiver_rank, int msg_tag);
MPI_Request send_first_row(int* src, int width, MPI_Datatype mpi_row, 
    int receiver_rank, int msg_tag);
MPI_Request send_last_row(int* src, int width, int height, MPI_Datatype mpi_row, 
    int receiver_rank, int msg_tag);

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
        MPI_COMM_WORLD, &request);
    return request;
}

MPI_Request send_right_column(int* src, int width, MPI_Datatype mpi_column, 
    int receiver_rank, int msg_tag) {
    MPI_Request request;
    MPI_Isend(src + 2 * width - 2, 1, mpi_column, receiver_rank, msg_tag, 
        MPI_COMM_WORLD, &request);
    return request;
}

MPI_Request send_first_row(int* src, int width, MPI_Datatype mpi_row, 
    int receiver_rank, int msg_tag) {
    MPI_Request request;
    MPI_Isend(src + width + 1, 1, mpi_row, receiver_rank, msg_tag, MPI_COMM_WORLD, 
        &request);
    return request;
}

MPI_Request send_last_row(int* src, int width, int height, MPI_Datatype mpi_row, 
                        int receiver_rank, int msg_tag) {
    MPI_Request request;
    MPI_Isend(src + width * (height - 1) - width + 1, 1, mpi_row, receiver_rank, 
        msg_tag, MPI_COMM_WORLD, &request);
    return request;
}

MPI_Request send_corner_pixel(int* src, int width, int height, 
    int receiver_rank, corner given_corner) {
    MPI_Request request;
    switch (given_corner) {
        case upper_left:
            MPI_Isend(src + width + 1, 1, MPI_INT, receiver_rank, 
                lower_right, MPI_COMM_WORLD, &request);
            break;
        case upper_right:
            MPI_Isend(src + 2 * width - 2, 1, MPI_INT, receiver_rank, 
                lower_left, 
                MPI_COMM_WORLD, &request);
            break;
        case lower_right:
            MPI_Isend(src + width * (height - 1) - 2, 1, MPI_INT, 
                receiver_rank, upper_left, MPI_COMM_WORLD, &request);
            break;
        case lower_left:
            MPI_Isend(src + width * (height - 1) - width + 1, 1, MPI_INT, 
                receiver_rank, upper_right, MPI_COMM_WORLD, &request);
            break;
    }
    return request;
}

int send_data(int* src, MPI_Datatype mpi_column, MPI_Datatype mpi_row, 
            int my_rank, int proc_num, int width, int height) {
    int tag = 10111;
    send_first_row(src, width, mpi_row, 0 , tag);
    send_last_row(src, width, height, mpi_row, 0 , tag);
    send_left_column(src, width, mpi_column, 0, tag);
    send_right_column(src, width, mpi_column, 0, tag);
    send_corner_pixel(src, width, height, 0, lower_left);
    send_corner_pixel(src, width, height, 0, lower_right);
    send_corner_pixel(src, width, height, 0, upper_left);
    send_corner_pixel(src, width, height, 0, upper_right);
    return 0;
}

