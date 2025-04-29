#include <math.h>
#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define PI 3.14159265358979323846

int main(int argc, char* argv[]) {
    int n = 0, m = 0, m_total = 0, m_recv = 0;
    float x, y, pi, err;
    double start_time, end_time, time_spent;

    srand(time(NULL));

    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    for (int it = 1; it <= 5; it++) {
        n = 100 * pow(10, it) / size;  // number of points/process

        start_time = MPI_Wtime();

        for (int i = 1; i <= n; i++) {
            x = (float)rand()/(float)(RAND_MAX);
            y = (float)rand()/(float)(RAND_MAX);
        
            // Check if the generated point is inside the unit circle.
            if (x * x + y * y <= 1) {
                m++;
            }
        }
    
        // Reduce all partial counts to the root process
        MPI_Reduce(&m, &m_total, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    
        end_time = MPI_Wtime();
        time_spent = end_time - start_time;
    
        if (rank == 0) {
            pi = 4.0 * (double)m_total / (size * n);
    
            err = fabs(PI - pi);
        
            printf("%i,%i,%f,%f\n", n, size, err, time_spent);
        }

    }

    MPI_Finalize();

    return 0;
}