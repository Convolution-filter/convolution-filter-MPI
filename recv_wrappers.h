/* 
 * File:   recv_wrappers.h
 * Author: chris
 *
 * Created on January 12, 2015, 9:25 PM
 */

#ifndef RECV_WRAPPERS_H
#define	RECV_WRAPPERS_H

#ifdef	__cplusplus
extern "C" {
#endif
    
MPI_Request* recv_data(int* buffer, MPI_Datatype mpi_column, 
    MPI_Datatype mpi_row, int width, int height, int msg_tag, int my_rank);
void wait_on_recv(MPI_Request* requests);


#ifdef	__cplusplus
}
#endif

#endif	/* RECV_WRAPPERS_H */

