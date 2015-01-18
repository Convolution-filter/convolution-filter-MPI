#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <string.h>
#include <math.h>
#include "initializations.h"
#include "processing_img.h"

//-------------------------------------------
// Forward declarations
//-------------------------------------------
int* BW_initialization(const char* filename, int width, int height,
        int img_block_width, int img_block_height, int proc_num);
int** RGB_initialization(const char* filename, int width, int height);
int** send_image(int* img_buffer, int width, int height, int proc_num, 
                int img_block_width, int img_block_height,
                int proc_per_row, MPI_Request** requests);
int* extract_img_block(int* img, int img_length, int proc_per_row,
            int block_id, int img_block_width, int img_block_height);
int** read_RGB_img(const char* filename, int width, int height);
int initialize_MPI();
int* read_BW_img(const char* filename, int width, int height);

//-------------------------------------------
// Datatypes 
//-------------------------------------------
MPI_Datatype mpi_column, mpi_row, mpi_block_img, mpi_block;


//-------------------------------------------
// Methods
//-------------------------------------------
int* initalization_phase(const char* filename, int width, int height, 
                        unsigned int isBW) {
    int proc_num = initialize_MPI();
    
    int proc_per_row;
    proc_per_row = (int) sqrt(proc_num);
    int img_block_width = width / proc_per_row;
    int img_block_height = height / proc_per_row;
    
    // Create datatype for columns
    MPI_Type_vector(img_block_height, 1, img_block_width + 2, MPI_INT, 
        &mpi_column);
    MPI_Type_commit(&mpi_column);
    MPI_Type_vector(1, img_block_width, 0, MPI_INT, &mpi_row);  
    MPI_Type_commit(&mpi_row);
    MPI_Type_vector(1, img_block_width * img_block_height, 0, MPI_INT, 
        &mpi_block_img);
    MPI_Type_commit(&mpi_block_img);
    MPI_Type_vector(img_block_height, img_block_width, 
            img_block_width + 2, MPI_INT, &mpi_block);
    MPI_Type_commit(&mpi_block);
    
    // Start separating master and slave procs
    int rank;
    int* block = NULL;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        if (isBW) {
            printf("Master: about to initialize BW\n");
            int* img_buffer = 
                    BW_initialization(filename, width, height,
                        img_block_width, img_block_height, proc_num);
            block = malloc((img_block_width + 2) * 
                        (img_block_height + 2) * sizeof(int));
            MPI_Scatter(img_buffer, 1, mpi_block_img, 
                block + img_block_width + 3, 1, mpi_block, 
                0, MPI_COMM_WORLD);
        }
    }
    else {
        if (isBW) {
            int block_length = 
                    (img_block_width + 2) * (img_block_height + 2);
            block = malloc(block_length * sizeof(int));
            memset(block, '\0', block_length * sizeof(int));
            MPI_Scatter(NULL, 1, mpi_block_img, block + img_block_width + 3, 
                1, mpi_block, 0, MPI_COMM_WORLD);
        }
    }
    return block;
}

int* BW_initialization(const char* filename, int width, int height, 
                        int img_block_width, int img_block_height,
                        int proc_num) {
    // Read image
//    int* img_buffer = read_BW_img(filename, width, height);
//    if (img_buffer == NULL)
//        return IMG_LOAD_FAILURE;
    int* img_buffer = create_random_array(width, height);
    return img_buffer;
}

/*
 * Extracts a block from the image (the block denoted by block_id)
 */
int* extract_img_block(int* img, int img_length, int proc_per_row,
            int block_id, int img_block_width, int img_block_height) {
    int i,j;
    int current_row = block_id / proc_per_row;
    int current_col = block_id - current_row * proc_per_row;
    int start_idx =
        current_row * img_block_height * img_block_width * proc_per_row
                    + current_col * img_block_width;
    printf("Start_idx == %d\n", start_idx);
    printf("max_idx == %d\n", img_block_width * img_block_height);
    int* buffer = NULL;
    buffer = malloc(img_block_width * img_block_height * sizeof(int));
    memset(buffer, '\0', img_block_width * img_block_height * sizeof(int));
    // In the for loop, insert the -1 pixel from the start
    // and the +1 from the end
    int idx = 0;
    for (i = start_idx, j = 1; i < img_length && j<=img_block_width*img_block_height; i++,j++) {
        buffer[idx] = img[i];
        idx++;
        if (j%img_block_width==0 && j!=0){
            i += img_block_width * (proc_per_row - 1);
            continue;
        }
    }
    return buffer;
}

int** RGB_initialization(const char* filename, int width, int height) {
    int** img_buffers = read_RGB_img(filename, width, height);
    if (img_buffers == NULL)
        return IMG_LOAD_FAILURE;
    return img_buffers;
}

/*
 * Initializes the MPI framework
 */
int initialize_MPI() {
    int numprocs;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    return numprocs;
}

/*
 * Send image blocks to all other processes
 */
int** send_image(int* img_buffer, int width, int height, int proc_num, 
                int img_block_width, int img_block_height,
                int proc_per_row, MPI_Request** requests) {
    int i;
    int** img_blocks = malloc((proc_num - 1) * sizeof(int*));
    *requests = malloc((proc_num - 1) * sizeof(MPI_Request));
    for (i = 1; i < proc_num; i++) {
        int* img_block = extract_img_block(img_buffer, width * height,
                        proc_per_row, i, img_block_width, img_block_height);
        MPI_Isend(img_block, img_block_width * img_block_height, mpi_block_img, 
            i, IMG_BLOCK_MSG, MPI_COMM_WORLD, &((*requests)[i - 1]));
    }
    return img_blocks;
}

/*
 * Will send the appropriate image blocks to the other processes
 */

/*
 * Reads the BW image into an int[] buffer 
 * Return value:
 *  On success, the buffer is returned
 *  Otherwise, NULL is returned
 */
int* read_BW_img(const char* filename, int width, int height) {
    FILE* img_file = fopen(filename, "rb");
    if (img_file == NULL) {
        fprintf(stderr, "read_BW_img - Could open image file\n");
        return NULL;
    }
    int *img = NULL;
    img = malloc(width * height * sizeof(int));
    if (img == NULL) {
        fprintf(stderr, "read_BW_img - Could not allocate img buffer\n");
        return NULL;
    }
    if (fread(img, 1, width * height, img_file) < width * height) {
        fprintf(stderr, "read_BW_img - Sth went wrong with image reading\n");
        free(img);
        img = NULL;
        return NULL;
    }
    fclose(img_file);
    return img;
}

/*
 * Reads the RGB image into 3 int[] buffers and returns an (int*)[] that
 * points to these 3 buffers
 * Return value:
 *  On success, the buffer mentioned above
 *  On failure, NULL is returned
 */
int** read_RGB_img(const char* filename, int width, int height) {
    FILE* img_file = fopen(filename, "rb");
    if (img_file == NULL) {
        fprintf(stderr, "read_RGB_img - Could open image file\n");
        return NULL;
    }
    int *img = NULL;
    img = malloc(width * height * 3);
    if (img == NULL) {
        fprintf(stderr, "read_RGB_img - Could not allocate img buffer\n");
        return NULL;
    }
    if (fread(img, 1, width * height * 3, img_file) < width * height) {
        fprintf(stderr, "read_RGB_img - Sth went wrong with image reading\n");
        free(img);
        img = NULL;
        fclose(img_file);
        return NULL;
    }
    fclose(img_file);
    
    int** rgb_buffers = malloc(3 * sizeof(int*));
    if (rgb_buffers == NULL) {
        fprintf(stderr, "read_RGB_img - Failed to allocate space for "
            "buffers\n");
        free(img);
        img = NULL;
        return NULL;
    }
    int* red = malloc(width * height);
    if (red == NULL) {
        fprintf(stderr, "read_RGB_img - Failed to allocate space for "
            "buffers\n");
        free(img);
        free(rgb_buffers);
        img = NULL;
        rgb_buffers = NULL;
        return NULL;
    }
    int* green = malloc(width * height);
    if (green == NULL) {
        fprintf(stderr, "read_RGB_img - Failed to allocate space for "
            "buffers\n");
        free(img);
        free(rgb_buffers);
        free(red);
        img = NULL;
        rgb_buffers = NULL;
        red = NULL;
        return NULL;
    }
    int* blue = malloc(width * height);
    if (blue == NULL) {
        fprintf(stderr, "read_RGB_img - Failed to allocate space for "
            "buffers\n");
        free(img);
        free(rgb_buffers);
        free(red);
        free(green);
        img = NULL;
        rgb_buffers = NULL;
        red = NULL;
        green = NULL;
        return NULL;
    }
    int i;
    for (i = 0; i < width * height * 3; i += 3) {
        red[i] = img[i];
        green[i + 1] = img[i + 1];
        blue[i + 2] = img[i + 2];
    }
    rgb_buffers[0] = red;
    rgb_buffers[1] = green;
    rgb_buffers[2] = blue;
    return rgb_buffers;
}
