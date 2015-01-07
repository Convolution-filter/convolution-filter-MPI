
#include "functions.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define filter_size 3

// Forward declarations
int calculate_filtered_pixel(int pixel_idx, int* src_array, int row_size, 
                                int* filter);

int* create_random_array(int row_size)
{
    int array_size = row_size * row_size;
    int* array = malloc(array_size * sizeof(int));
    int i;
    srand(time(NULL));
    for ( i = 0; i < array_size; i++)
    {
        int shade = rand() % 255;
        array[i] = shade;
//        array[i] = 1;
    }

    return array;
}


void compute_internal_values(int* src_array, int* dest_array, int row_size, int* filter)
{
    int i, row_number = 3;
    int array_size = row_size * row_size;
    for (i = 2 * row_size + 2; i < array_size - 2 * row_size - 2; i++)
    {
        if( i % (row_number * row_size - 2) == 0)
        {
            //to skip elements of the first and last column
            i += 3;
            row_number++;
            continue;
        }
        dest_array[i] = calculate_filtered_pixel(i, src_array, row_size, filter);
    }
}


void compute_outer_values(int* src_array, int* dest_array, int row_size, int* filter) 
{
    int i;
    int array_size = row_size * row_size;
    // Compute outer lines
    for (i = row_size + 1; i < array_size - row_size - 1; i++)
    {
        // Go to last line
        if (i % (2 * row_size - 1) == 0) {
            i = array_size - 2 * row_size;
            continue;
        }
        dest_array[i] = calculate_filtered_pixel(i, src_array, row_size, filter);
    }
    // Compute outer columns
    for (i = 2 * row_size + 1; i < array_size - 3 * row_size + 3; i += row_size)
    {
        dest_array[i] = calculate_filtered_pixel(i, src_array, row_size, filter);
        int right_idx = i + row_size - 3;
        dest_array[right_idx] = calculate_filtered_pixel(right_idx, src_array, row_size, filter);
    }
}


int calculate_filtered_pixel(int pixel_idx, int* src_array, int row_size, 
                                int* filter)
{
    int sum = 0;
    int j, z = 0;
    int coef = - 1;
    for (j = pixel_idx + coef * row_size - 1; 
         j <= pixel_idx + row_size + 1; j++ )
    {
        if(j % (row_size) == (pixel_idx + 2) % row_size)
        {
            coef++;
            j = pixel_idx + coef * row_size - 2;
            continue;
        }
        sum += src_array[j] * filter[z] / (filter_size * filter_size);
        z++;
    }
    return sum;
}


void print_array(int* arr, int row_size)
{
    int i;
    for (i = 0; i<row_size*row_size;i++)
    {
        if((i != 0) && (i % (row_size )== 0))
            printf("\n");
        printf("%4d ", arr[i]);
    }
    putchar('\n');
}
