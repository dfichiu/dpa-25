#!/bin/bash

# Compile the sequential and parallel versions
gcc -std=gnu17 -O3 -o seq seq.c -lm
mpicc -std=gnu17 -O3 -o prog p3.c -lm

# Output file for results
OUTPUT_FILE="results_2.txt"

# Clear the output file
> $OUTPUT_FILE

echo "Running sequential version..."
# echo "Sequential results (processes,matrix_size,execution_time):" > $OUTPUT_FILE


# for size in 128 256 512 1024 2048; do
#     echo "Running sequential with matrix size $size"
#     ./seq $size >> $OUTPUT_FILE
# done

echo "" >> $OUTPUT_FILE
echo "Parallel results (processes,matrix_size,execution_time,GFLOPS):" >> $OUTPUT_FILE

for np in $(seq 2 6); do
    echo "Running with -np $np"
    for size in 1024; do
        echo "Running parallel with -np $np and matrix size $size"
        mpirun -np $np ./prog $size >> $OUTPUT_FILE
    done
done
