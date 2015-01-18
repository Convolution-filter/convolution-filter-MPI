/* 
 * File:   communication_functions.h
 *
 */

#ifndef COMMUNICATION_FUNCTIONS_H
#define	COMMUNICATION_FUNCTIONS_H

#ifdef	__cplusplus
extern "C" {
#endif

    typedef enum direction {
        upper_left,
        upper,
        upper_right,
        right,
        lower_right,
        lower,
        lower_left,
        left
    } direction;
    
    MPI_Request* send_data(int* src, int my_rank, int proc_num, int width, int height);
    void wait_on_send(MPI_Request* requests);
    void assign_requests(MPI_Request* requests, int* idx, MPI_Request req_val);

#ifdef	__cplusplus
}
#endif

#endif	/* COMMUNICATION_FUNCTIONS_H */

