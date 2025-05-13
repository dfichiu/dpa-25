#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

// Default matrix size if not specified via command line
#define DEFAULT_MATRIX_SIZE 2048
#define BLOCK_SIZE 16

int main(int argc, char **argv) {
    int MATRIX_SIZE = DEFAULT_MATRIX_SIZE;
    
    // Allow matrix size to be specified via command line
    if (argc > 1) {
        MATRIX_SIZE = atoi(argv[1]);
    }
    
    // Dynamic allocation of matrices
    double **matrixA = (double **)malloc(MATRIX_SIZE * sizeof(double *));
    double **matrixB = (double **)malloc(MATRIX_SIZE * sizeof(double *));
    double **matrixResult = (double **)malloc(MATRIX_SIZE * sizeof(double *));
    
    for (int i = 0; i < MATRIX_SIZE; i++) {
        matrixA[i] = (double *)malloc(MATRIX_SIZE * sizeof(double));
        matrixB[i] = (double *)malloc(MATRIX_SIZE * sizeof(double));
        matrixResult[i] = (double *)malloc(MATRIX_SIZE * sizeof(double));
    }
    // Initialize matrices with the same values as in p3.c for fair comparison
    for (int i = 0; i < MATRIX_SIZE; ++i) {
        for (int j = 0; j < MATRIX_SIZE; ++j) {
            matrixA[i][j] = (double)(i + j);
            matrixB[i][j] = (double)(i * j);
            matrixResult[i][j] = 0.0;
        }
    }

    // Start timing
    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);
    
    // Matrix multiplication
    for (int i = 0; i < MATRIX_SIZE; ++i) {
        for (int j = 0; j < MATRIX_SIZE; ++j) {
            for (int k = 0; k < MATRIX_SIZE; ++k) {
                matrixResult[i][j] += matrixA[i][k] * matrixB[k][j];
            }
        }
    }
    
    // End timing
    gettimeofday(&end_time, NULL);
    
    // Calculate execution time in seconds
    double execTime = (end_time.tv_sec - start_time.tv_sec) + 
                      (end_time.tv_usec - start_time.tv_usec) / 1000000.0;
    
    // Print results in the same format as p3.c for easy comparison
    // Format: processes,matrix_size,execution_time,GFLOPS
    printf("1,%d,%f\n", MATRIX_SIZE, execTime);
    
    // Free allocated memory
    for (int i = 0; i < MATRIX_SIZE; i++) {
        free(matrixA[i]);
        free(matrixB[i]);
        free(matrixResult[i]);
    }
    free(matrixA);
    free(matrixB);
    free(matrixResult);
    
    return 0;
}
