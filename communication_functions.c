#include <mpi.h>
#include <math.h>
#include "communication_functions.h"

// Forward declarations
MPI_Request send_left_column(int* src, int width, MPI_Datatype mpi_column, 
    int receiver_rank, int msg_tag);
MPI_Request send_right_column(int* src, int width, MPI_Datatype mpi_column, 
    int receiver_rank, int msg_tag);
MPI_Request send_first_row(int* src, MPI_Datatype mpi_row, 
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
    MPI_Isend(src + width, 1, mpi_column, receiver_rank, msg_tag, 
        MPI_COMM_WORLD, &request);
    return request;
}

MPI_Request send_right_column(int* src, int width, MPI_Datatype mpi_column, 
    int receiver_rank, int msg_tag) {
    MPI_Request request;
    MPI_Isend(src + 2 * width - 1, 1, mpi_column, receiver_rank, msg_tag, 
        MPI_COMM_WORLD, &request);
    return request;
}

MPI_Request send_first_row(int* src, MPI_Datatype mpi_row, 
    int receiver_rank, int msg_tag) {
    MPI_Request request;
    MPI_Isend(src + 1, 1, mpi_row, receiver_rank, msg_tag, MPI_COMM_WORLD, 
        &request);
    return request;
}

MPI_Request send_last_row(int* src, int width, int height, MPI_Datatype mpi_row, 
    int receiver_rank, int msg_tag) {
    MPI_Request request;
    MPI_Isend(src + width * height - width + 1, 1, mpi_row, receiver_rank, 
        msg_tag, MPI_COMM_WORLD, &request);
    return request;
}

int send_data(int* src, MPI_Datatype mpi_column, MPI_Datatype mpi_row, 
    int my_rank, int proc_num,
    int width, int height) {
    int tag = 10111;
    send_left_column(src, width, mpi_column, 0, tag);
    send_right_column(src, width, mpi_column, 0, tag);
    send_first_row(src, mpi_row, 0 , tag);
    send_last_row(src, width, height, mpi_row, 0 , tag);
    return 0;
}

