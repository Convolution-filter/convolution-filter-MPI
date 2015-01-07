#include <stdio.h>
#include <stdlib.h>
#include "functions.h"

int main(void)
{
    int filter[10] = { 9, 9, 9, 9, 9, 9, 9, 9, 9, 9 };
    int * ar = create_random_array(10);
    print_array(ar,10);
    int* dest = malloc(100*sizeof(int));
    compute_internal_values(ar,dest,10,filter);
    putchar('\n');
    print_array(dest,10);



    return 0;
}
