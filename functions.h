
int* create_random_array(int array_size);

void compute_internal_values(int* from_array, int* destination_array, int row_size, int* filter);

void print_array(int* arr, int row_size);

void compute_outer_values(int* src_array, int* dest_array, int row_size, int* filter);
