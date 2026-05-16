#ifndef CUDA_WORKER_H
#define CUDA_WORKER_H

#ifdef __cplusplus
extern "C" {
#endif

void cuda_matrix_double(double *A, double *C, int N);

#ifdef __cplusplus
}
#endif

#endif