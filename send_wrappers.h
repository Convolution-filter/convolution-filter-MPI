/* 
 * File:   communication_functions.h
 *
 */

#ifndef COMMUNICATION_FUNCTIONS_H
#define	COMMUNICATION_FUNCTIONS_H

#ifdef	__cplusplus
extern "C" {
#endif

//    int send_data()
    typedef enum direction {
        up_left,
        up,
        up_right,
        right,
        down_right,
        down,
        down_left,
        left
    } direction;
    
    typedef enum corner {
        upper_left,
        upper_right,
        lower_right,
        lower_left
    } corner;
    
    int send_data(int* src, MPI_Datatype mpi_column, MPI_Datatype mpi_row, 
    int my_rank, int proc_num, int width, int height);

#ifdef	__cplusplus
}
#endif

#endif	/* COMMUNICATION_FUNCTIONS_H */

