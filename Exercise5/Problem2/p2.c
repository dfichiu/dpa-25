#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

void makeBipartite(int *V, int *E, int n, int m, int *V_b, int *E_b) {
    for (int i = 0; i < n; i++)
        V_b[i] = V[i];

    int b_curr_edge_idx = 0;
    for (int i = 1; i < n; i++) {
        int curr_V_offset = V[i - 1];
        int n_neighbors = V[i] - V[i - 1];

        for (int j = 0; j < n_neighbors; j++)
            E_b[b_curr_edge_idx++] = n + E[curr_V_offset + j];
    }
    
    for (int j = V[n - 1]; j < 2 * m; j++)
        E_b[b_curr_edge_idx++] = n + E[j];
}

void matchAndUpdate(
    int *V,
    int *E,
    int n,
    int m,
    int vIdx,
    int *visited,
    int *deg,
    int *M
) {
    int vertexVisits, neighborVisits, neighborsNeighborDeg;

    #pragma omp parallel
    {
        vertexVisits = __atomic_fetch_add(&visited[vIdx], 1, __ATOMIC_SEQ_CST);
    }

    // Check if vertex in X has already been visited.
    if (vertexVisits == 0) {
        /*
            Iterate over adjacent vertices.
        */ 
        int curr_V_offset = V[vIdx];
        int nNeighbors;
        if (vIdx == n - 1)
            nNeighbors = 2 * m - V[vIdx];
        else
            nNeighbors = V[vIdx + 1] - V[vIdx];

        for (int j = 0; j < nNeighbors; j++) {
            // Get neighbor.
            int neighborIdx = E[curr_V_offset + j];

            #pragma omp parallel
            {
                neighborVisits = __atomic_fetch_add(
                    &visited[neighborIdx],
                    1,
                    __ATOMIC_SEQ_CST
                );
            }

            // Check if the neighbor has been visited.
            if (neighborVisits == 0) {
                M[vIdx] = neighborIdx;
                M[neighborIdx] = vIdx;

            
                /*
                    Iterate over adjacent vertices of neighbour - check in first array.
                */ 
                int neighborIdxInOriginalGraph = neighborIdx - n;
                int curr_neighbor_offset = V[neighborIdxInOriginalGraph];
                int nNeighborsNeighbors;
                if (neighborIdxInOriginalGraph == n - 1)
                    nNeighborsNeighbors = 2 * m - V[neighborIdxInOriginalGraph];
                else
                    nNeighborsNeighbors = (
                        V[neighborIdxInOriginalGraph + 1] -
                        V[neighborIdxInOriginalGraph]
                    );
                for (int k = 0; k < nNeighborsNeighbors; k++) {
                    int neighborsNeighborIdx = E[curr_neighbor_offset + k];

                    #pragma omp parallel
                    {
                        neighborsNeighborDeg = __atomic_fetch_add(
                            &deg[neighborsNeighborIdx],
                            -1,
                            __ATOMIC_SEQ_CST
                        );
                    }

                    if (neighborsNeighborDeg == 2)
                        matchAndUpdate(
                            V,
                            E,
                            n,
                            m,
                            neighborsNeighborIdx,
                            visited,
                            deg,
                            M
                        );
                }
                break;
            }
        }
    }
}


void karpSipser(int *V, int *V_b, int *E, int nx, int ny, int m, int *M) {
    int *Q1, *Qs;

    Q1 = malloc(nx * sizeof(int));
	memset(Q1, 0, nx * sizeof(int));

    Qs = malloc(nx * sizeof(int));
	memset(Qs, 0, nx * sizeof(int));

    int *visited;
    visited = malloc((nx + ny) * sizeof(int));
	memset(visited, 0, (nx + ny) * sizeof(int));

    int *deg;
    deg = malloc((nx + ny) * sizeof(int));
    
    for (int i = 1; i < nx; ++i)
        deg[i - 1] = V[i] - V[i - 1];
    deg[nx - 1] = 2 * m - V[nx - 1];

    for (int i = nx + 1; i < nx + ny; ++i)
        deg[i - 1] = V_b[i - nx] - V_b[i - nx - 1];
    deg[nx + ny - 1] = 2 * m - V_b[nx - 1];

    for (int vIdx = 0; vIdx < nx; vIdx++) {
        if (V[vIdx + 1] - V[vIdx] == 1)
            Q1[vIdx] = 1;
        else  
            Qs[vIdx] = 1;
    }

    for (int vIdx = 0; vIdx < nx; vIdx++) 
        if (Q1[vIdx] == 1)
            matchAndUpdate(
                V,
                E,
                nx,
                m,
                vIdx,
                visited,
                deg,
                M
            );

    for (int vIdx = 0; vIdx < nx; vIdx++) 
        if (Qs[vIdx] == 1)
            matchAndUpdate(
                V,
                E,
                nx,
                m,
                vIdx,
                visited,
                deg,
                M
            );
}


int main(int argc, char* argv[]) {
	FILE * fp;
	char* line = NULL;
	size_t len = 0;
	ssize_t read;
	
	int n, m;

	int edge_idx = 0;
	
	int *V, *V_b;
	int *E, *E_tmp, *E_b;

    char *l;
	
	bool firstline = false;

    fp = fopen("Exercise4/Problem3/data/CAG_mat1916.mtx", "r");
	
    /*
        Part I: Construct undirected graph.
    */
	while ((read = getline(&line, &len, fp)) != -1) {
		if (line[0] != '%' && !firstline) {
			l = strtok(line, " ");
			
			n = atoi(l);
			l = strtok(NULL, " ");
			l = strtok(NULL, " ");
			m = atoi(l);
			
			V = malloc(n * sizeof(int));
			memset(V, 0, n * sizeof(int));
			
			E = malloc(2 * m * sizeof(int));
			memset(E, -1, 2 * m * sizeof(int));

			E_tmp = malloc(2 * m * sizeof(int));
			
			// printf("%d, %d \n", n, m);
			firstline = true;	
		} else if (line[0] != '%') {
			int left, right, tmp;

			l = strtok(line, " ");
			left = atoi(l) - 1;

			l = strtok(NULL, " ");
			right = atoi(l) - 1;
			
			E_tmp[edge_idx] = left;
			E_tmp[edge_idx + 1] = right;
			
			edge_idx += 2;	
		}
	}
	
    // Prefix to compute starting indices in E.
	for (int i = 0; i < 2 * m; i++)
		if (E_tmp[i] < n - 1)
		    V[E_tmp[i] + 1]++;

	for (int i = 1; i < n; i++)
		V[i] += V[i - 1];

	for (int i = 0; i < 2 * m; i += 2) {
		int curr_edge_offset = V[E_tmp[i]];

		while (E[curr_edge_offset] != -1)
			curr_edge_offset++;

		E[curr_edge_offset] = E_tmp[i + 1];
	}

	for (int i = 0; i < 2 * m; i += 2) {
		int curr_edge_offset = V[E_tmp[i + 1]];

		while (E[curr_edge_offset] != - 1)
			curr_edge_offset++;

		E[curr_edge_offset] = E_tmp[i];
	}
	
	free(E_tmp);

    /*
        Part II: Construct bipartite graph.
    */
    V_b = malloc(n * sizeof(int));
	memset(V_b, 0, n * sizeof(int));

    E_b = malloc(2 * m * sizeof(int));

    makeBipartite(V, E, n, m, V_b, E_b);

    // for (int i = 0; i < 2 * m; i++)
    //     printf("%d ", E_b[i]);

    int *M;
    M = malloc(2 * n * sizeof(int));
    memset(M, -1, 2 * n * sizeof(int));

    /*
        Part III: Construct maximal matching.
    */
    karpSipser(
        V,
        V_b,
        E_b,
        n,
        n,
        m,
        M
    );

    // for (int i = 0; i < 2 * n; i++)
    //     printf("%d ", M[i]);
    
    return 0;
}