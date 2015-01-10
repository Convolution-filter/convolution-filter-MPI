#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "functions.h"
#include "hello_mpi.h"

int main()
{
//    int block_size = 10;
//    int filter[9] = { 1, 2, 1, 2, 4, 2, 1, 2, 1 };
//    int * ar = create_random_array(block_size);
//    print_array(ar,block_size);
//    int* dest = malloc(block_size * block_size * sizeof(int));
//    compute_internal_values(ar,dest,block_size,filter);
//    compute_outer_values(ar, dest, block_size, filter);
//    putchar('\n');
//    print_array(dest,block_size);
    hello_mpi();
    return 0;
}
