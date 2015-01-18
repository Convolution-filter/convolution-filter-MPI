#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "functions.h"

#define filter_size 3

// Forward declarations
int calculate_filtered_pixel(int pixel_idx, int* src_array, int width,
    int height, int* filter);

int* create_random_array(int width, int height)
{
    int array_size = width * height;
    int* array = malloc(array_size * sizeof(int));
    int i;
    srand(time(NULL));
    for ( i = 0; i < array_size; i++)
    {
//        int shade = rand() % 255;
//        array[i] = shade;
        array[i] = i;
    }

    return array;
}


void compute_internal_values(int* src_array, int* dest_array, 
                                int width, int height, int* filter)
{
    int i, row_number = 3;
    int array_size = width * height;
    for (i = 2 * width + 2; i < array_size - 2 * width - 2; i++)
    {
        if( i % (row_number * width - 2) == 0)
        {
            //to skip elements of the first and last column
            i += 3;
            row_number++;
            continue;
        }
        dest_array[i] = calculate_filtered_pixel(i, src_array, width, height, filter);
    }
}


void compute_outer_values(int* src_array, int* dest_array, 
    int width, int height, int* filter) 
{
    int i;
    int array_size = width * height;
    // Compute outer lines
    for (i = width + 1; i < array_size - width - 1; i++)
    {
        // Go to last line
        if (i % (2 * width - 1) == 0) {
            i = array_size - 2 * width;
            continue;
        }
        dest_array[i] = 
            calculate_filtered_pixel(i, src_array, width, height, filter);
    }
    // Compute outer columns
    for (i = 2 * width + 1; i < array_size - 3 * width + 3; i += width)
    {
        dest_array[i] = 
            calculate_filtered_pixel(i, src_array, width, height, filter);
        int right_idx = i + width - 3;
        dest_array[right_idx] = 
            calculate_filtered_pixel(right_idx, src_array, width, height, filter);
    }
}


int calculate_filtered_pixel(int pixel_idx, int* src_array, int width,
    int height, int* filter)
{
    int sum = 0;
    int j, z = 0;
    int coef = - 1;
    for (j = pixel_idx + coef * width - 1; 
         j <= pixel_idx + width + 1; j++ )
    {
        if ((j % width) == (pixel_idx + 2) % width)
        {
            coef++;
            j = pixel_idx + coef * width - 2;
            continue;
        }
        sum += src_array[j] * filter[z] / (filter_size * filter_size);
        z++;
    }
    return sum;
}


void print_array(int* arr, int width, int height)
{
    int i;
    for (i = 0; i< width * height;i++)
    {
        if((i != 0) && (i % width == 0))
            printf("\n");
        printf("%4d ", arr[i]);
    }
    putchar('\n');
}
