# Distributed matrix multiplication (OpenMPI)

Multiply matrix by vector on multiple MPI nodes.

MPI will first divide the process into a bunch of sub-processes.
Then process with rank 0 will read the matrix and vector from files passed as arguments.

Process with rank 0 will send the matrix row by row to process with rank 1 excepted the first ones.
Then process will rank 1 will send the received lines to process with rank 2 excepted the first ones.
This method is called **token ring**.

Next processes will use the same method to share the vector.

Each process will multiply the received lines by the vector and send the result to process with rank below his.
With the same token ring method, the result will be sent to process with rank 0.

At the end, process with rank 0 will concat the results into a single 1D matrix and print the result.


## Usage

```bash
git clone https://github.com/thomasarmel/matrix_multiplication_openmpi.git
cd matrix_multiplication_openmpi
cmake .
make
mpirun -np <number of processes> ./matmult <matrix file> <vector file> [<output file>]
```

By default, output file is `output.txt`.