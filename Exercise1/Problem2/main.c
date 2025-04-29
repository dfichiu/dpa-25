#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <omp.h>

#define PI 3.14159265358979323846

float approximate_pi(int n) {
    float x, y;
    int m = 0;

    #pragma omp parallel for private(x, y) reduction(+:m)
    for (int i = 0; i < n; i++) {
        // Use thread-local seed for rand_r
        unsigned int seed = (unsigned int)(time(NULL)) ^ (omp_get_thread_num() + i);
        x = (float)rand_r(&seed) / (float)RAND_MAX;
        y = (float)rand_r(&seed) / (float)RAND_MAX;

        if (x * x + y * y <= 1) {
            m++;
        }
    }

    return 4.0 * ((float)m / (float)n);
}

int main(int argc, char* argv[]) {
    int n_max = 10000000;
    float pi, err;
    double time_spent;
    clock_t begin, end;

    for (int n = 10; n <= n_max; n *= 10) {
        begin = clock();
        pi = approximate_pi(n);
        end = clock();

        time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
        err = fabs(PI - pi);

        printf("%i,%f,%f\n", n, err, time_spent);
    }

    return 0;
}
