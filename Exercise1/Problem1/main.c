#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define PI 3.14159265358979323846

float approximate_pi(int n) {
    // Set set of generator.
    srand((unsigned int) time(NULL));

    float x, y;
    int m = 0;

    for (int i = 0; i < n; i++) {
        // Generate point's coordinates, i.e., sample two random values from U[0, 1].
        x = (float)rand()/(float)(RAND_MAX);
        y = (float)rand()/(float)(RAND_MAX);

        // Check if the generated point is inside the unit circle.
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