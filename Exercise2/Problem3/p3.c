#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <mpi.h>

// #define MATRIX_SIZE 5

int main(int argc, char **argv) {
    int rank, size;
    int countSlaves, countRowSlave, restRows, toProcessRows;
    int rowOffset;
    int MATRIX_SIZE;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MATRIX_SIZE = atoi(argv[1]);

    /* Dynamic alllocation of A, B, and C (which is set to 0). */
    double **A = malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(double *));
    A[0] = malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(double));
    for (int i = 1; i < MATRIX_SIZE; ++i)
        A[i] = A[0] + i * MATRIX_SIZE;

    double **B = malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(double *));
    B[0] = malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(double));
    for (int i = 1; i < MATRIX_SIZE; ++i)
        B[i] = B[0] + i * MATRIX_SIZE;

    double **C = calloc(MATRIX_SIZE * MATRIX_SIZE, sizeof(double *));
        C[0] = calloc(MATRIX_SIZE * MATRIX_SIZE, sizeof(double));
        for (int i = 1; i < MATRIX_SIZE; ++i)
            C[i] = C[0] + i * MATRIX_SIZE;

    countSlaves = size - 1;
    countRowSlave = MATRIX_SIZE / countSlaves;
    restRows = MATRIX_SIZE % countSlaves;

    if (rank == 0) { /* Master */
        double start_time, end_time; 

        // Initialize A & B.
        for (int i = 0; i < MATRIX_SIZE; ++i) {
            for (int j = 0; j < MATRIX_SIZE; ++j) {
                A[i][j] = (double) (i + j);
                B[i][j] = (double) (i * j);
            }
        }

        // Set row offset to 0.
        rowOffset = 0;

        MPI_Barrier(MPI_COMM_WORLD); // Set barrier for synchronisation.

        /* Send data for computation to slaves. */ 
        for (int dest = 1; dest <= countSlaves; ++dest) {
            /* Use the rank of the slave process as tag. */
            // Send B.
            start_time = MPI_Wtime();
            MPI_Send(&(B[0][0]), MATRIX_SIZE * MATRIX_SIZE, MPI_DOUBLE, dest, dest, MPI_COMM_WORLD);
            // Send number of rows to process.
            toProcessRows = countRowSlave;
            if (dest == countSlaves) {
                // The last slave gets the rest of the rows.
                toProcessRows += restRows;
            }
            MPI_Send(&toProcessRows, 1, MPI_INT, dest, dest, MPI_COMM_WORLD);
            // Send offset of first row to be processed.
            MPI_Send(&rowOffset, 1, MPI_INT, dest, dest, MPI_COMM_WORLD);
            // Send rows to be processed.
            MPI_Send(&(A[rowOffset][0]), toProcessRows * MATRIX_SIZE, MPI_DOUBLE, dest, dest, MPI_COMM_WORLD); 

            rowOffset += toProcessRows;
        }

        /* Fill in C with data computed by slaves. */
        for (int source = 1; source <= countSlaves; ++source) {
            // Receive numbers of processed rows.
            MPI_Recv(&toProcessRows, 1, MPI_INT, source, source, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            // Receive offset of first processed row.
            MPI_Recv(&rowOffset, 1, MPI_INT, source, source, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            // Receive processed rows.
            MPI_Recv(&(C[rowOffset][0]), toProcessRows * MATRIX_SIZE, MPI_DOUBLE, source, source, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        end_time = MPI_Wtime();
        // print for 3.4
        // printf("%d,%d,%f\n", size, MATRIX_SIZE, end_time - start_time);

        double execTime = end_time - start_time;
        double GFLOPS = (2.0 * MATRIX_SIZE * MATRIX_SIZE * MATRIX_SIZE) / (execTime * 1e9);

        // print for 3.5
        printf("%d,%d,%f,%f\n", size, MATRIX_SIZE, execTime, GFLOPS);

        /* Output C. */
        // for (int i = 0; i < MATRIX_SIZE; ++i) {
        //     for (int j = 0; j < MATRIX_SIZE; ++j) {
        //         printf("%lf ", C[i][j]);
        //     }
        //     printf("\n");
        // }
        
    } else { /* Slave */
        /* Receive data for computation from master. */ 
        int source = 0;

        MPI_Barrier(MPI_COMM_WORLD); // Set barrier for synchronisation.

        // Receive B.
        MPI_Recv(&(B[0][0]), MATRIX_SIZE * MATRIX_SIZE, MPI_DOUBLE, source, rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // Receive numbers of rows to process.
        MPI_Recv(&toProcessRows, 1, MPI_INT, source, rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // Receive offset of first row to be processed.
        MPI_Recv(&rowOffset, 1, MPI_INT, source, rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // Receive rows to slave.
        MPI_Recv(&(A[rowOffset][0]), toProcessRows * MATRIX_SIZE, MPI_DOUBLE, source, rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        /* Compute rows of C. */
        for (int i = rowOffset; i < rowOffset + toProcessRows; ++i) {
            for (int j = 0; j < MATRIX_SIZE; ++j) {
                for (int k = 0; k < MATRIX_SIZE; ++k) {
                    C[i][j] += A[i][k] * B[k][j];
                }
            }
        }

        /* Send computed rows of C back to master. */
        // Send number of computed rows.
        MPI_Send(&toProcessRows, 1, MPI_INT, source, rank, MPI_COMM_WORLD);
        // Send offset of first processed row.
        MPI_Send(&rowOffset, 1, MPI_INT, source, rank, MPI_COMM_WORLD);
        // Send processed rows to master.
        MPI_Send(&(C[rowOffset][0]), toProcessRows * MATRIX_SIZE, MPI_DOUBLE, source, rank, MPI_COMM_WORLD); 
    }

    MPI_Finalize();
    return 0;
}




    
