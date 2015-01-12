int* create_random_array(int width, int height);

void compute_internal_values(int* from_array, int* destination_array, 
        int width, int height, int* filter);

void compute_outer_values(int* src_array, int* dest_array, 
        int width, int height, int* filter);

void print_array(int* arr, int width, int height);

