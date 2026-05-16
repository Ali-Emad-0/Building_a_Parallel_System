#include <cuda_runtime.h>
#include "cuda_worker.h"

/* CUDA Kernel */
__global__ void matrix_double_kernel(double *A,
                                     double *C,
                                     int N)
{
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    if(row < N && col < N)
        C[row*N + col] = 2.0 * A[row*N + col];
}

/* CUDA Host Function */
void cuda_matrix_double(double *A, double *C, int N)
{
    double *d_A, *d_C;
    size_t size = N*N*sizeof(double);

    cudaMalloc(&d_A,size);
    cudaMalloc(&d_C,size);

    cudaMemcpy(d_A,A,size,cudaMemcpyHostToDevice);

    dim3 block(16,16);
    dim3 grid((N+15)/16,(N+15)/16);

    matrix_double_kernel<<<grid,block>>>(d_A,d_C,N);

    cudaDeviceSynchronize();

    cudaMemcpy(C,d_C,size,cudaMemcpyDeviceToHost);

    cudaFree(d_A);
    cudaFree(d_C);
}