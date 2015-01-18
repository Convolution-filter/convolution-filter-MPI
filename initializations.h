/* 
 * File:   initializations.h
 */

#ifndef INITIALIZATIONS_H
#define	INITIALIZATIONS_H

#ifdef	__cplusplus
extern "C" {
#endif

typedef enum errors {
    IMG_LOAD_FAILURE
} errors;


int* initalization_phase(const char* filename, int width, int height, 
                        unsigned int isBW);



#ifdef	__cplusplus
}
#endif

#endif	/* INITIALIZATIONS_H */

