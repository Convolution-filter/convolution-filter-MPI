int* create_random_array(int width, int height);

int* process_img(int* block, int block_width, int block_height, int rep_num);

void compute_inner_values(int* from_array, int* destination_array, 
        int width, int height, int* filter);

void compute_outer_values(int* src_array, int* dest_array, 
        int width, int height, int* filter);

void print_array(int* arr, int width, int height);

