#include <stdio.h>
#include <stdlib.h>
#include <time.h>
// #include <omp.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#define MAX 1000000

void remove_vertex(int vIdx, int n, int* V, int m, int* E) {
    int vOffset = V[vIdx];
    int nNeighbors = 0, vNeighbors = 0;

    // Get number of neighbors of v.
    if (vIdx == n - 1)
        vNeighbors = 2 * m - V[vIdx];
    else
        vNeighbors = V[vIdx + 1] - vOffset;

    for (int i = 0; i < vNeighbors; i++) {
        // Get neighbor.
        int nIdx = E[vOffset + i];

        // Remove edge with neighbor from vertex's perspective by setting it to -1.
        E[vOffset + i] = -1;

        // Get neighbor's offset.
        int nOffset = V[nIdx];

        // Get number of neighbors of n.
        if (nIdx == n - 1)
            nNeighbors = 2 * m - V[nIdx];
        else
            nNeighbors = V[nIdx + 1] - nOffset;

        // Remove edge with neighbor from neighbor's perspective
        // by setting it to -1.
        for (int j = 0; j < nNeighbors; j++)
            if (E[nOffset + j] == vIdx)
                E[nOffset + j] = -1;
                break;
    }
}

int get_vertex_degree(int vIdx, int n, int* V, int m, int* E) {
    int vDeg = 0;
    int nNeighbors = 0;

    // Get vertex offset.
    int vOffset = V[vIdx];

    // Get initial number of neighbors (prior to any removals).
    if (vIdx == n - 1)
        nNeighbors = 2 * m - V[vIdx];
    else
        nNeighbors = V[vIdx + 1] - vOffset;

    // Compute degree by couting the number of currently existing neighbors.
    for (int i = 0; i < nNeighbors; i++)
        if (E[vOffset + i] != -1)
            vDeg++;

    return vDeg;
}

int get_minimal_degree(int n, int* V, int m, int* E) {
    int nNeighbors;
    int minDeg = MAX;

    for (int vIdx = 0; vIdx < n; vIdx++) {
        // Check if the vertex hasn't already been removed.
        int vDeg = get_vertex_degree(vIdx, n, V, m, E);
        
        if (vDeg > 0 && vDeg < minDeg)
            minDeg = vDeg;
        
    }

    return minDeg;
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

    fp = fopen("data/road_usa.mtx", "r");
	
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

    int currPrio = 0;
    int currPrioTaken = 0;
    
    int d = get_minimal_degree(n, V, m, E);
    int currNoOfVertices = n;

    int* Q;
    Q = malloc(n * sizeof(int));
    memset(Q, 0, n * sizeof(int));

    int *q;
    q = malloc(n * sizeof(int));

    while (currNoOfVertices > 0) {
        for (int vIdx = 0; vIdx < n; vIdx++)
            if (Q[vIdx] != -1) 
                Q[vIdx] = get_vertex_degree(vIdx, n, V, m, E);

        // printf("d = %d\n", d);
        for (int vIdx = 0; vIdx < n; vIdx++) {
            if (Q[vIdx] != -1 && Q[vIdx] <= d) {
                // printf("vIdx = %d\n", vIdx);
                if (currPrioTaken == 0) {
                    currPrioTaken = 1;
                    q[vIdx] = currPrio;
                } else {
                    q[vIdx] = vIdx;
                }

                // Remove processed vertex from V.
                remove_vertex(vIdx, n, V, m, E);
                Q[vIdx] = -1;
                currNoOfVertices--;
            }
        }

        // Increase current priority.
        currPrio++;
        currPrioTaken = 0;

        // Recompute d.
        int dNew = get_minimal_degree(n, V, m, E);
        d = (d + 1 > dNew) ? d + 1 : dNew;
        // int d = get_minimal_degree(n, V, m, E);
    }

    return 0;
}