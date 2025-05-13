#include <math.h>
#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char* argv[]) {
    int N = 100;
    int* message;
    int tag = 0;
    double start_time, end_time, elapsed_time, max_time;

    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    message = (int*)malloc(N * sizeof(int));

    if (rank == 0) {
        srand(time(NULL));
        // Initialize message.
        for (int j = 0; j < N; j++) {
            message[j] = rand();
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    start_time = MPI_Wtime();

    if (rank > 0) {
        int i = 0;
        while ((rank >> i) % 2 == 0) {
            i++;
        }
        int source = rank - (1 << i);
        MPI_Recv(message, N, MPI_INT, source, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("%d received message: [%d, %d, %d]\n", rank, message[N - 1], message[N - 2], message[N - 3]);
    }
    for (int k = ceil(log2(size)) - 1; k >= 0; k--) {
        int dest = rank + pow(2, k);
        if (rank % (1 << k + 1) == 0 && dest < size) {
            MPI_Send(message, N, MPI_INT, dest, tag, MPI_COMM_WORLD);
        }
    }

    end_time = MPI_Wtime();
    elapsed_time = end_time - start_time;

    MPI_Reduce(&elapsed_time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Binomial tree broadcast completed in %f seconds (max across all ranks)\n", max_time);
    }
    
    free(message);
    MPI_Finalize();

    return 0;
}