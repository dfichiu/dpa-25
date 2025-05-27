#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <math.h>
#include <stdbool.h>



int main(){

	unsigned int n = 20000;
	
	int arr[n];
	
	int bucket[2 * n];

	srand(time(NULL));


	//Init Array
	
	for(int i = 0; i < n; i++){
		arr[i] = rand() % 10000;
	}
	
	//Begin Time Measure
	
	double start = omp_get_wtime();
	for(int i = 0; i < 2*n; i++){
		bucket[i] = -1;
	}
	
	bucket[0] = 0;
	bucket[1] = n - 1;
	
	int numblocks = 1;
	int tempblocks = numblocks;
	
	//RadixSort. Iterate over all bits.
	for(int bit = 63; bit >= 0; bit--){
		unsigned int b = 1 << bit;
			
		#pragma omp parallel for
		for(int block = 0; block < numblocks; block++){ // Blocks/Buckets of numbers with the same prefix.
			if(bucket[2 * block] == bucket[2 * block + 1]) continue;
			int compare = bucket[2 * block + 1];
			for(int i = bucket[2 * block]; i <= bucket[2 * block + 1]; i++){
				if((arr[i] & b) == 0){ //If the bit is 0 continue..
					continue;
				}
				
				if(compare != i){ //look from the end of the array to find an element that has a 0-bit and switch it if it exists.
					while(((arr[compare] & b) != 0) && compare > i){
						compare--;
					}
					if((arr[compare] & b) == 0){
						arr[i] = arr[i] ^ arr[compare];
						arr[compare] =  arr[i] ^ arr[compare];
						arr[i] = arr[i] ^ arr[compare];
					}
				
				}
				
				if(compare == i){
					if(i != bucket[2 * block]) { //Create a new block/bucket if the current bucket has at least one number with a bit 1 at position b and at least one number with a bit 0 at position b
						#pragma omp critical
						{
						bucket[tempblocks * 2] = i;
						bucket[tempblocks * 2 + 1] = bucket[2 * block + 1] ;  
						tempblocks++;
												
						bucket[2 * block + 1] = i - 1;
						}
					} 
					
					break;
				
				} 
			}
		}
		numblocks = tempblocks;
	}
	
	double end = omp_get_wtime();
	printf("TIME: %f\n", end-start);
	
	
	bool sorted = true;
	for(int i = 0; i < n - 1; i++){
		if(arr[i] > arr[i+1]){
			sorted = false;
		}
	}
	if(sorted) printf("CORRECTLY SORTED");
	else printf("NOT CORRECTLY SORTED");
	
	
	return 0;

	
}
