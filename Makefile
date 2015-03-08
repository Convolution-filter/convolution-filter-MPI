CC = mpicc
FLAGS = -c
LINK_FLAGS = -lm
OBJS = main.o send_wrappers.o recv_wrappers.o initializations.o processing_img.o
EXECUTABLE_NAME = parallel-convolution
OUTPUT = output.raw

# Compile
all: $(OBJS)
ifdef OPEN_MP
	$(CC) $(OBJS) -fopenmp $(LINK_FLAGS) -o $(EXECUTABLE_NAME)
else
	$(CC) $(OBJS) $(LINK_FLAGS) -o $(EXECUTABLE_NAME)
endif
	rm -f $(OBJS)

main.o: main.c
	$(CC) $(FLAGS) main.c

send_wrappers.o: send_wrappers.c
	$(CC) $(FLAGS) send_wrappers.c

recv_wrappers.o: recv_wrappers.c
	$(CC) $(FLAGS) recv_wrappers.c

initializations.o: initializations.c
	$(CC) $(FLAGS) initializations.c

processing_img.o: processing_img.c
ifdef OPEN_MP
	$(CC) $(FLAGS) -D OMP -fopenmp processing_img.c
else
	$(CC) $(FLAGS) processing_img.c
endif
# Clean-up
clean:
	rm -f $(EXECUTABLE_NAME)
clean-all:
	rm -rf $(EXECUTABLE_NAME) $(OUTPUT)

# Usage
help:
	@:
		$(info To run with OpenMP: <make all OPEN_MP=1>)
		$(info if not, simply use: <make all>)
