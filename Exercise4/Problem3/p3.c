#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define INF 10000

int main(int argn, char* argv[]) {
	FILE *fp;
	char *line = NULL;
    char *l = NULL;
    size_t len = 0;
	ssize_t read;
	
	int n;
	int *d, *d_copy;
    int s, t, w;

    int n_it = 10;
	
    fp = fopen("data/CAG_mat1916.mtx", "r");
	bool firstline = false;
	
    // Read in file.
	while ((read = getline(&line, &len, fp)) != -1) {
		if (line[0] != '%' && !firstline) {
            // Tokenize line, get first token.
			l = strtok(line, " ");
			
            // Convert token to int.
			n = atoi(l);
			
            // Initialize distance matrix.
			d = malloc(n * n * sizeof(int));

            for (int i = 0; i < n; i++)
                for (int j = 0; j < n; j++)
                    d[i * n + j] = INF;

			// printf("%d\n", n);
			firstline = true;
		} else if (line[0] != '%') {
			l = strtok(line, " ");
			s = atoi(l) - 1;

			l = strtok(NULL, " ");
			t = atoi(l) - 1;

            l = strtok(NULL, " ");
			w = atoi(l);
			
			d[s * n + t] = w;
		}
	}

    // Set diagonal to 0.
    for (int i = 0; i < n; i++)
        d[i * n + i] = 0;

    d_copy = malloc(n * n * sizeof(int));
    for (int i = 0; i < n * n; i++)
        d_copy[i] = d[i];

    double total_exec_time = 0;
    // Perform experiment.
    for (int it = 0; it < n_it; it++) {
        if (it > 0)
            for (int i = 0; i < n * n; i++)
                d[i] = d_copy[i];

        double itime, ftime;
        itime = omp_get_wtime();
        for (int k = 0; k < n; k++)
            #pragma omp parallel for
            for (int i = 0; i < n; i++)
                for (int j = 0; j < n; j++) 
                        // Compute new minimum distance accounting for vertex k.
                        if (d[i * n + j] > d[i * n + k] + d[k * n + j])
                            d[i * n + j] = d[i * n + k] + d[k * n + j];

        ftime = omp_get_wtime();
        total_exec_time += ftime - itime;
    }

    printf("%f", total_exec_time / n_it);
    // for (int i = 0; i < n; i++) {
    //     for (int j = 0; j < n; j++) {
    //         printf("%d ", d[i * n + j]);   
    //     }
    //     printf("\n");
    // }

    return 0;
}