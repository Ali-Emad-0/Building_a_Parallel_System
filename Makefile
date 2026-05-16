CC=mpicc
NVCC=nvcc

CFLAGS=-fopenmp
TARGET=program

all: $(TARGET)

cuda_worker.o: cuda_worker.cu
	$(NVCC) -c cuda_worker.cu

$(TARGET): Building_a_Parallel_System.c cuda_worker.o
	$(CC) $(CFLAGS) Building_a_Parallel_System.c cuda_worker.o -lcudart -o $(TARGET)

clean:
	rm -f *.o $(TARGET)