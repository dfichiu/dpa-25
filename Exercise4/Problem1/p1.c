#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

int main(int argc, char* argv[]){

	//Read Input
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
	
	
	
	//Sequential
	int * depthv = malloc(n * sizeof(int));
	memset(depthv, -1, n* sizeof(int));
	{
	double itime, ftime, exec_time;
    	itime = omp_get_wtime();
	int * queue = malloc(n * sizeof(int));
	bool * visited = malloc(n * sizeof(int));
	
	memset(visited, false, n* sizeof(bool));
	
	int count = 0;
	int appended = 1;
	
	int depth = 0;
	int level = 1;
	int nextLevel = 0;
	
	queue[0] = 0;
	visited[0] = true;
	
	while(count != appended){
		int vertex = queue[count];
		count++;
		level--;
		depthv[vertex] = depth;
		for(int neighbouridx = ver[vertex]; neighbouridx < ver[vertex + 1]; neighbouridx++){
			if(!visited[edges[neighbouridx]]) {
				nextLevel++;
				queue[appended] = edges[neighbouridx];
				appended++;
				visited[edges[neighbouridx]] = true;

			}
			
		}
		
		if(level == 0){
			level = nextLevel;
			nextLevel = 0;
			depth++;
		}
	}
	
	
    	ftime = omp_get_wtime();
	exec_time = ftime - itime;
   	printf("\n\nSequential Time taken is %f", exec_time);
	
	free(queue);
	free(visited);
	
	}
	
	
	//PARALLEL ALGORITHM
	
	double itime, ftime, exec_time;
    	itime = omp_get_wtime();

	int maxdegree = 0;
	
	int count = 0;
 
	for(int i = 0; i < n - 1;){
		int j = i + 1;
		while(ver[j] == -1 && j < n){
			j++;
		}
		if(maxdegree < ver[j] - ver[i]) maxdegree = ver[j] - ver[i];	
		i = j;
	}

	
	int * depthv2 = malloc(n * sizeof(int));
	int * queue = malloc(n * sizeof(int));
	bool * visited = malloc(n * sizeof(int));
	
	memset(visited, false, n* sizeof(bool));

	
	int depth = 0;
	int left_idx = 0;
	int right_idx = 1;
	
	bool empty = false;
	
	visited[0] = true;
	queue[0] = 0;
	depthv2[0] = 0;

	
	while(!empty) {

		int size = right_idx - left_idx;
		int nOfBlocks = 4; //Number of Threads
		if(size < nOfBlocks) nOfBlocks = size;	
		int blocksize = size / 	nOfBlocks;
		omp_set_num_threads(nOfBlocks);

		int offset[nOfBlocks + 1];
		memset(offset, 0, (nOfBlocks + 1) * sizeof(int));

		#pragma omp parallel
		{

			int * neighbours = malloc(maxdegree * blocksize * sizeof(int));
			
			int count = 0; 

			int id = omp_get_thread_num();
			
			#pragma omp for
			for(int i = left_idx; i < right_idx; i++){
				int vertex = queue[i];
				depthv2[vertex] = depth;
				for(int neighbours_idx = ver[vertex]; neighbours_idx < ver[vertex + 1]; neighbours_idx++) {
					
					{							
						if(__sync_bool_compare_and_swap(&visited[edges[neighbours_idx]], false, true)){
							neighbours[count++] = edges[neighbours_idx];
						}														
					}
				}
			}
			
			#pragma omp critical
			{
				for(int i = id + 1; i < nOfBlocks + 1; i++){
					offset[i] += count;
				}
			}

			#pragma omp barrier
			
			for(int i = 0; i < count; i++){
				queue[right_idx + offset[id] + i] = neighbours[i];
				
			}
			#pragma omp barrier
			free(neighbours);
			#pragma omp master
			{	
				left_idx = right_idx;

				right_idx = left_idx + offset[nOfBlocks];
				if(left_idx == right_idx) empty = true;
				
				depth++;
			}
		
		}
	
	}
	

	ftime = omp_get_wtime();
	exec_time = ftime - itime;
   	printf("\n\nParallel Time taken is %f \n", exec_time);
   	
   	for(int i = 0; i < n; i++){
   		
   		if(depthv[i] != depthv2[i]) {
   		
   			 printf("Different Depth %d, %d, %d \n", i, depthv[i], depthv2[i]);
   		}
   	}
	return 0;
}
