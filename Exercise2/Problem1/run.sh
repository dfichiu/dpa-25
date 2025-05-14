#!/bin/bash  

OUTPUT_FILE="results.csv"  
 
> $OUTPUT_FILE  

for np in $(seq 1 6); do  
    echo "Running with -np $np"  
    mpirun -np $np ./prog 10

    arg=100  
    while [ $arg -le 10000000 ]; do  
        echo "Running with argument $arg"  
        mpirun -np $np ./prog $arg >> $OUTPUT_FILE
        arg=$((arg * 10))
    done
done  