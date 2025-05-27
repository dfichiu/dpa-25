#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <math.h>
#include <stdbool.h>



int main(){

	unsigned int n = 100000;
	
	int arr[n];
	
	int bucket[2 * n];

	srand(time(NULL));


	//Init Array
	for(int i = 0; i < n; i++){
		arr[i] = rand() % 10000;
	}
	
	
	double start = omp_get_wtime();
	for(int i = 0; i < 2*n; i++){
		bucket[i] = -1;
	}
	bucket[0] = 0;
	bucket[1] = n - 1;
	
	int numblocks = 1;
	int tempblocks = numblocks;
	
	//RadixSort
	for(int bit = 63; bit >= 0; bit--){
		unsigned int b = 1 << bit;
			
	/*printf("%d\n", bit);
	

	for(int i = 0; i < n; i++){
		bool outb = (arr[i] & b) != 0; 

	
		printf("arr[%d]: %d Bit: %d\n", i, arr[i], outb);
	}
	
	for(int i = 0; i < 2*n; i++){
		printf("block[%d]=%d\n", i, bucket[i]);
	}*/
	
	//fflush(stdout);
		#pragma omp parallel for
		for(int block = 0; block < numblocks; block++){
			if(bucket[2 * block] == bucket[2 * block + 1]) continue;
			//printf("block %d\n", block);
			int compare = bucket[2 * block + 1];
			for(int i = bucket[2 * block]; i <= bucket[2 * block + 1]; i++){
				if((arr[i] & b) == 0){ //If the bit is 0 continue..
					continue;
				}
				
				if(compare != i){
					while(((arr[compare] & b) != 0) && compare > i){
						compare--;
					}
					if((arr[compare] & b) == 0){
						#pragma omp critical
						//printf("Switching %d with %d\n", i, compare);
						arr[i] = arr[i] ^ arr[compare];
						arr[compare] =  arr[i] ^ arr[compare];
						arr[i] = arr[i] ^ arr[compare];
					}
				
				}
				
				if(compare == i){
					//printf("break, old block %d from %d to %d\n", block, bucket[2 * block],  i - 1);
					//printf("break, new block  %d from %d to %d\n", block, i ,bucket[2 * block + 1] );
					if(i != bucket[2 * block]) {
						bucket[tempblocks * 2] = i;
						bucket[tempblocks * 2 + 1] = bucket[2 * block + 1] ;  
						tempblocks++;
												
						bucket[2 * block + 1] = i - 1;
					} 
					
					break;
				
				} else {
					
					

				}
					
				
				
			}
		}
		numblocks = tempblocks;
	}
	
	double end = omp_get_wtime();
	//float seconds = (float)(end - start) / CLOCKS_PER_SEC;
	printf("TIME: %f\n", end-start);
	
	
	bool sorted = true;
	for(int i = 0; i < n - 1; i++){
		if(arr[i] > arr[i+1]){
			sorted = false;
		}
	}
	if(sorted) printf("CORRECTLY SORTED");
	else printf("NOT CORRECTLY SORTED");
	
	
	
	/*
	#pragma omp parallel
	{
		unsigned int randomState = clock();
		#pragma omp for reduction (+:m)
		for(int i=0; i < nOfP; i++){
		    //printf("Hello from process: %d\n", omp_get_thread_num());
			float x = (rand_r(&randomState) % 1000) / 1000.0;
			float y = (rand_r(&randomState) % 1000) / 1000.0;
			if(x * x + y * y <= 1) {
				m++;
			}
		}
	}
	
	double pi = 4.0f * m / (double)nOfP;
	//printf("%d\n", nOfP);
	//printf("%f\n", pi);
	
	double end = omp_get_wtime();
	//float seconds = (float)(end - start) / CLOCKS_PER_SEC;
	printf("%d %f\n", nOfP, end-start);*/
	
	return 0;

	
}
