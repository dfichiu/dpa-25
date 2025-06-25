#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>


void induced_subgraph(int *V, int *E, int n, int *S, int *V_s, int *E_s, int *n_s);

int main(int argc, char* argv[]){
	FILE * fp;
	char* line = NULL;
	size_t len = 0;
	ssize_t read;
	
	fp = fopen("road_usa/road_usa.mtx", "r");
	
	int n, m;
	
	int current_vertex = 0;
	int edge_idx = 0;
	
	int * ver;
	int * edges;
	int * temp_edges;
	
	bool firstline = false;
		
	while((read = getline(&line, &len, fp)) != -1){
		if(line[0] != '%' && !firstline) {
			
			char * l = strtok(line, " ");
			
			n = atoi(l);
			l = strtok(NULL, " ");
			l = strtok(NULL, " ");
			m = atoi(l);
			
			ver = malloc((n + 1) * sizeof(int));
			memset(ver, 0, (n + 1) * sizeof(int));
			ver[n] = 2 * m;
			
			edges = malloc(2 * m * sizeof(int));
			memset(edges, -1, 2 * m * sizeof(int));
			temp_edges = malloc(2 * m * sizeof(int));
			
			printf("%d, %d \n", n, m);
			firstline = true;	
			

		} else if (line[0] != '%'){
			
			int neighbour;
			char * l = strtok(line, " ");
			neighbour = atoi(l);
			l = strtok(NULL, " ");
			int c = atoi(l);
			
			temp_edges[edge_idx] = neighbour - 1;
			temp_edges[edge_idx + 1] = c - 1;
			
			edge_idx += 2;
			
		}
	}
	
	
	
	for(int i = 0; i < 2 * m; i ++){
		if(temp_edges[i] == n - 1) continue;
		ver[temp_edges[i] + 1]++;
	}
	for(int i = 1; i < n; i++){
		ver[i] += ver[i - 1];
	}
	
	for(int i = 0; i < 2 * m; i += 2){
		int currentedgeidx = ver[temp_edges[i]];

		while(edges[currentedgeidx] != -1){
			currentedgeidx++;
		}
		edges[currentedgeidx] = temp_edges[i + 1];
	}
	
	
	for(int i = 0; i < 2 * m; i+=2){
		int currentedgeidx = ver[temp_edges[i + 1]];
		while(edges[currentedgeidx] != - 1){
			currentedgeidx++;
		}
		edges[currentedgeidx] = temp_edges[i];
	}
	
	free(temp_edges);
	
	int * S = malloc(n * sizeof(int)); 
	int n_s;
	int * V_s;
	int * E_s;
	V_s = malloc((n + 1) * sizeof(int));
	E_s = malloc(2 * m * sizeof(int));
	
	for(int i = 0; i < n; i++){
		S[i] = rand() % 2;
	}
	
	double itime, ftime, exec_time;
    	itime = omp_get_wtime();
	
	induced_subgraph(ver, edges, n, S, V_s, E_s, &n_s);
		
	ftime = omp_get_wtime();
	exec_time = ftime - itime;
   	printf("\n\nParallel Time taken is %f \n", exec_time);
   	
   	
}


/*int main(int argc, char* argv[]){
	FILE * fp;
	char* line = NULL;
	size_t len = 0;
	ssize_t read;
	
	fp = fopen("CAG_mat1916/CAG_mat1916.mtx", "r");
	
	int n, m;
	
	int current_vertex = 0;
	int edge_idx = 0;
	
	int * ver;
	int * edges;
	
	int edge_counter = 0;
	
	bool firstline = false;
		
	while((read = getline(&line, &len, fp)) != -1){
		if(line[0] != '%' && !firstline) {
			
			char * l = strtok(line, " ");
			
			n = atoi(l);
			l = strtok(NULL, " ");
			l = strtok(NULL, " ");
			m = atoi(l);
			
			ver = malloc((n + 1) * sizeof(int));
			memset(ver, -1, (n + 1) * sizeof(int));
			ver[0] = 0;
			ver[n] = m;
			
			edges = malloc(m * sizeof(int));
			memset(edges, -1, m * sizeof(int));
			
			printf("%d, %d \n", n, m);
			firstline = true;	
			

		} else if (line[0] != '%'){
			
			int neighbour;
			char * l = strtok(line, " ");
			neighbour = atoi(l);
			l = strtok(NULL, " ");
			int c = atoi(l) - 1;
			l = strtok(NULL, " ");
			edges[edge_counter++] = neighbour - 1;
			
			if(c != current_vertex) {
				current_vertex = c;
				ver[c] = edge_counter - 1; 
			}			
		}
	}
	
	for(int i = n - 1; i >= 0; i--) {
		if(ver[i] == -1) {
			ver[i] = ver[i - 1];
		}
	}


	
	int * S = malloc(n * sizeof(int)); 
	int n_s;
	int * V_s;
	int * E_s;
	V_s = malloc((n + 1) * sizeof(int));
	E_s = malloc(m * sizeof(int));
	
	for(int i = 0; i < n; i++){
		S[i] = rand() % 2;
	}
	
	double itime, ftime, exec_time;
    	itime = omp_get_wtime();
	
	induced_subgraph(ver, edges, n, S, V_s, E_s, &n_s);
		
	ftime = omp_get_wtime();
	exec_time = ftime - itime;
   	printf("\n\nParallel Time taken is %f \n", exec_time);
}*/

void induced_subgraph(int *V, int *E, int n, int *S, int *V_s, int *E_s, int *n_s){
	
	int m_s = 0;
	int total_v = 0;
	int total_e = 0;
	
	int * translation = malloc(n * sizeof(int));
	
	int * offset = malloc((omp_get_num_threads() + 1) * sizeof(int));
	int * offset_e = malloc((omp_get_num_threads() + 1) * sizeof(int));
		
	memset(offset, 0, (omp_get_num_threads() + 1)* sizeof(int));
	memset(offset_e, 0, (omp_get_num_threads() + 1)* sizeof(int));
	
	translation[0] = S[0];
	
	for(int i = 1; i < n; i++){
		translation[i] = translation[i-1] + S[i];
	}
	
	
	
	#pragma omp parallel 
	{
		int * local_V_s = malloc(n * sizeof(int));
		int * local_E_s = malloc(50 *n * sizeof(int));
		int local_n_s = 0;
		int local_m_s = 0;
		int thread_id = omp_get_thread_num();
		int blocksize = n / omp_get_num_threads();
		
		local_V_s[0] = 0;
		
		int upperbound = ((thread_id + 1) * blocksize < n)? (thread_id + 1) * blocksize : n;
		
		for(int i = thread_id * blocksize ; i < upperbound; i++){
			if(S[i] == 1) {
				for(int j = V[i]; j < V[i + 1]; j++){
					if(S[E[j]] == 1){
					
						local_E_s[local_m_s] = translation[E[j]];								
						local_m_s++;
						
					}
				}
					
				local_n_s++;
				local_V_s[local_n_s] = local_m_s;		
			}						
		}


		int start_idx_v = 0;
		int start_idx_e = 0;
		#pragma omp critical
		{
		
			for(int i = omp_get_thread_num() + 1; i <= omp_get_num_threads(); i++){
				offset[i] += local_n_s;
				offset_e[i] += local_m_s;
			}
			
			//start_idx_v = total_v;
			total_v += local_n_s;
			//start_idx_e = total_e;
			total_e += local_m_s;
		}
		#pragma omp barrier
		#pragma omp single
		{
			*n_s = total_v;
			V_s[total_v] = total_e;
		}
		
		for(int i = 0; i < local_n_s; i++){
			V_s[offset[thread_id] + i] = local_V_s[i] + offset[thread_id];
		}
		
		for(int i = 0; i < local_m_s; i++){
			E_s[offset_e[thread_id] + i] = local_E_s[i];
		}
		
		free(local_V_s);
		free(local_E_s);

		
		
	}	
}
