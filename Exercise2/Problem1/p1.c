#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

int main(int argc, char* argv[]){


	int process_rank, size_of_cluster;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size_of_cluster);
	MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);


	int n = 1000;
	//sscanf(argv[1], "%d", &n);

	int* array = malloc(n * sizeof(int));


	
	if(process_rank == 0){

		clock_t start = clock();

		srand(time(NULL));

		for(int i = 0; i < n; i++){
			array[i] = rand() % 1000;
		}


		for(int i = 1; i < size_of_cluster; i++){
			MPI_Send(array, n, MPI_INT, i, 0, MPI_COMM_WORLD);
		}

		clock_t end = clock();
		float seconds = (float)(end - start) / CLOCKS_PER_SEC;
		printf("%f", seconds);

		
	} else  {
		MPI_Recv(array, n, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		int sum = 0;
		
		for(int i = 0; i < n; i++){
			sum += (array[i] & 1 << 3) + (array[i] & 1 << 2) + (array[i] & 1 << 1);
		}
		
		printf("%d %d\n", process_rank, sum);
	}
	
	free(array);
	
	MPI_Finalize();
	
	
}
