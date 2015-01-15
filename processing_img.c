#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <mpi.h>
#include <string.h>
#include "processing_img.h"
#include "send_wrappers.h"
#include "recv_wrappers.h"

#define filter_sum 16
int filter[9] = { 1, 2, 1, 2, 4, 2, 1, 2, 1 };

// Forward declarations
int calculate_filtered_pixel(int pixel_idx, int* src_array, int width,
    int height, int* filter);

int* create_random_array(int width, int height)
{
    int array_size = width * height;
    int* array = malloc(array_size * sizeof(int));
    int i;
    srand(time(NULL));
    for (i = 0; i < array_size; i++)
    {
//        int shade = rand() % 255;
//        array[i] = shade;
        array[i] = i;
    }

    return array;
}


int* process_img(int* block, int block_width, int block_height, int rep_num) {
    int i, rank, proc_num;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &proc_num);
    MPI_Request* requests_send, *requests_recv;
    int* tmp_block = malloc(block_width * block_height * sizeof(int));
    memset(tmp_block, '\0', block_width * block_height * sizeof(int));
    for (i = 0; i < rep_num; i++) {
        // Send outer
//        printf("proc(%d) - before send\n", rank);
        requests_send = send_data(block, rank, proc_num, block_width, block_height);
        // Recv foreign
//        printf("proc(%d) - before recv\n", rank);
        requests_recv = recv_data(block, rank, proc_num, block_width, block_height);
        // Process our (inner) block
//        printf("proc(%d) - before inner compute\n", rank);
        compute_inner_values(block, tmp_block, block_width, block_height, filter);
        // Wait for our foreign
//        printf("proc(%d) - before wait on recv\n", rank);
        wait_on_recv(requests_recv);
        // Process our (outer) block
//        printf("proc(%d) - before outer compute\n", rank);
        compute_outer_values(block, tmp_block, block_width, block_height, filter);
        // Wait on send of our outer
//        printf("proc(%d) - before wait on send\n", rank);
        wait_on_send(requests_send);
        //sleep(rank * (rank + 2));
        //printf("proc(%d) finished\n", rank);
        //print_array(block, block_width, block_height);
        memcpy(block, tmp_block, block_width * block_height * sizeof(int));
    }
//    sleep(rank * (rank + 2));
//    printf("proc(%d) finished\n", rank);
//    print_array(block, block_width, block_height);
    return block;
}


void compute_inner_values(int* src_array, int* dest_array,
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
//        dest_array[i] = src_array[i];
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
//        dest_array[i] = src_array[i];
    }
    // Compute outer columns
    for (i = 2 * width + 1; i < array_size - 3 * width + 3; i += width)
    {
        dest_array[i] =
            calculate_filtered_pixel(i, src_array, width, height, filter);
//        dest_array[i] = src_array[i];
        int right_idx = i + width - 3;
//        dest_array[right_idx] = src_array[right_idx];
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
        sum += (src_array[j] * filter[z]) / filter_sum;
        z++;
    }
    if (sum > 255)
        return 255;
    else
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
