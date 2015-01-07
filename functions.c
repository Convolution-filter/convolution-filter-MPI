
#include "functions.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define filter_size 3

int* create_random_array(int row_size)
{
    int array_size = row_size * row_size;
    int* array = malloc(array_size * sizeof(int));
    int i;
    srand(time(NULL));
    for ( i = 0; i < array_size; i++)
    {
        //int shade = rand()%255;
        //array[i] = shade;
        array[i] = 1;
    }

    return array;
}


int compute_internal_values(int* src_array, int* dest_array, int row_size, int* filter)
{
    int i, row_number = 3;
    int array_size = row_size * row_size;
    for (i = 2 * row_size + 2; i < array_size - 2 * row_size - 2; i++)
    {
        if( i % (row_number * row_size - 2) == 0)
        {//to skip elements of the first and last column
            i += 3;
            row_number++;
            continue;
        }
        int sum = 0;
        int j, z = 0;
        int coef = - 1;
        for ( j = i + coef * row_size - 1; j <= i + row_size + 1; j++ )
        {
            if(j % (row_size) == (i + 2) % row_size)
            {
                coef++;
                j = i + coef * row_size - 2;
                continue;
            }

            //sum += src_array[j] * filter[z] / (filter_size * filter_size);
            sum +=  filter[z] / (filter_size * filter_size);
            z++;
        }
        dest_array[i] = sum;
    }
    return 0;
}

void print_array(int* arr, int row_size)
{
    int i;
    for (i = 0; i<row_size*row_size;i++)
    {
        if((i != 0) && (i % (row_size )== 0))
            printf("\n");
        printf("%3d ", arr[i]);
    }
    putchar('\n');
}
