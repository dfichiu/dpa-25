#!/bin/bash

# Compile the sequential and parallel versions
gcc -std=gnu17 -O3 -o seq seq.c -lm
mpicc -std=gnu17 -O3 -o prog p3.c -lm

# Output file for results
OUTPUT_FILE="results_2.txt"

# Clear the output file
> $OUTPUT_FILE

echo "Running sequential version..."
echo "Sequential results (processes,matrix_size,execution_time):" > $OUTPUT_FILE


# for size in 128 256 512 1024 2048; do
#     echo "Running sequential with matrix size $size"
#     ./seq $size >> $OUTPUT_FILE
# done

echo "" >> $OUTPUT_FILE
echo "Parallel results (processes,matrix_size,execution_time,GFLOPS):" >> $OUTPUT_FILE

for np in $(seq 2 6); do
    echo "Running with -np $np"
    for size in 128 256 512 1024 2048; do
        echo "Running parallel with -np $np and matrix size $size"
        mpirun -np $np ./prog $size >> $OUTPUT_FILE
    done
done

echo "All tests completed. Results saved to $OUTPUT_FILE"

cat > plot_results.py << 'EOF'
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Read the results file
with open('results.txt', 'r') as f:
    lines = f.readlines()

# Parse the sequential and parallel results
seq_results = []
par_results = []
parsing_sequential = True

for line in lines:
    line = line.strip()
    if not line or line.startswith("Sequential") or line.startswith("Parallel"):
        if line.startswith("Parallel"):
            parsing_sequential = False
        continue
    
    parts = line.split(',')
    if len(parts) == 4:
        processes = int(parts[0])
        matrix_size = int(parts[1])
        exec_time = float(parts[2])
        gflops = float(parts[3])
        
        if parsing_sequential:
            seq_results.append((processes, matrix_size, exec_time, gflops))
        else:
            par_results.append((processes, matrix_size, exec_time, gflops))

# Convert to DataFrames
seq_df = pd.DataFrame(seq_results, columns=['Processes', 'MatrixSize', 'ExecTime', 'GFLOPS'])
par_df = pd.DataFrame(par_results, columns=['Processes', 'MatrixSize', 'ExecTime', 'GFLOPS'])

# Plot execution time vs matrix size for different number of processes
plt.figure(figsize=(12, 6))

# Plot sequential results
for size in seq_df['MatrixSize'].unique():
    seq_time = seq_df[seq_df['MatrixSize'] == size]['ExecTime'].values[0]
    plt.axhline(y=seq_time, linestyle='--', alpha=0.5, 
                label=f'Sequential (Size={size})')

# Plot parallel results grouped by matrix size
for size in par_df['MatrixSize'].unique():
    size_df = par_df[par_df['MatrixSize'] == size]
    plt.plot(size_df['Processes'], size_df['ExecTime'], 'o-', 
             label=f'Parallel (Size={size})')

plt.xlabel('Number of Processes')
plt.ylabel('Execution Time (seconds)')
plt.title('Matrix Multiplication Performance: Sequential vs Parallel')
plt.grid(True, alpha=0.3)
plt.legend()
plt.savefig('exec_time_comparison.png')

# Plot GFLOPS vs matrix size for different number of processes
plt.figure(figsize=(12, 6))

# Plot sequential results
for size in seq_df['MatrixSize'].unique():
    seq_gflops = seq_df[seq_df['MatrixSize'] == size]['GFLOPS'].values[0]
    plt.axhline(y=seq_gflops, linestyle='--', alpha=0.5, 
                label=f'Sequential (Size={size})')

# Plot parallel results grouped by matrix size
for size in par_df['MatrixSize'].unique():
    size_df = par_df[par_df['MatrixSize'] == size]
    plt.plot(size_df['Processes'], size_df['GFLOPS'], 'o-', 
             label=f'Parallel (Size={size})')

plt.xlabel('Number of Processes')
plt.ylabel('GFLOPS')
plt.title('Matrix Multiplication Performance: Sequential vs Parallel')
plt.grid(True, alpha=0.3)
plt.legend()
plt.savefig('gflops_comparison.png')

print("Plots generated: exec_time_comparison.png and gflops_comparison.png")
EOF

echo "A Python plotting script has been created. Run 'python3 plot_results.py' after the tests to generate performance comparison plots."
