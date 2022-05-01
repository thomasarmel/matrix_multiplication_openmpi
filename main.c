#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include "process_transmissions.h"
#include "matrix_operations.h"
#include "read_write_file.h"

void MPI_exit(int code);

/**
 * @brief Main function
 * @param argc
 * @param argv Parameters
 * @return 0 if everything went well, a positive value otherwise
 * @note Cost analysing (without latency):
 * Let's suppose we run the program using a square matrix of size NxN, and P processes simultaneously.
 * Reading and writing the matrix isn't taken into account.
 * So first of all, process 0 will have to scatter the matrix to all the other processes.
 * Because matrix is sent line by line, first process will have to send N - N/P lines, so N * (N - N/P) integers.
 * The second process will have to send N * (N - 2N/P) integers, so N^2 * (1 - 2/P) integers.
 * Finally, the mean complexity per process is O(N ^ 2)
 * And the total cost of scatter is O(P * N^2)
 *
 * For sharing the multiplication vector of size N, each process except the last one has to send it to the next process.
 * So the cost per process is O(N), the total cost is O(P * N)
 *
 * After calculating the partial matrix multiplication, each process will have to send the result to the previous process.
 * Result is one integer per matrix line.
 * So then the process 0 will gather the results from all the other processes.
 * The last process will have to send 1 * N/P integers to the previous one.
 * This process, after receiving the result, will have to send his result to the previous process.
 * So N/P + N/P integers.
 * In average, the cost per process is O(N)
 * So the total cost is O(P * N)
 *
 *
 * Finally, the total cost of the program is O(P * N^2) + O(P * N) + O(P * N) = O(P * N^2)
 */
int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    int rank, numprocs;
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    const char *OUTPUT_VECTOR_FILENAME = "output.txt"; // Default output file name

    if (argc < 3)
    {
        if (rank == 0)
        {
            printf("Usage: %s <input_file> <output_file>\n", argv[0]);
        }
        MPI_exit(1);
    }

    if (numprocs < 2)
    {
        if (rank == 0)
        {
            fprintf(stderr, "Need at least 2 processes\n");
        }
        MPI_exit(5);
    }

    Matrix2D *scattered_matrix2D = NULL;
    Matrix1D *multiply_vector = NULL;
    const char *MATRIX_FILENAME = argv[1]; // Input matrix file name
    const char *VECTOR_FILENAME = argv[2]; // Input vector file name
    if (argc > 3) // Output file passed to the command line
    {
        OUTPUT_VECTOR_FILENAME = argv[3];
    }

    if (rank == 0)
    {
        Matrix2D *entry_matrix2D = read_matrix_from_file(MATRIX_FILENAME);
        if (entry_matrix2D == NULL)
        {
            fprintf(stderr, "Error reading matrix from file %s\n", MATRIX_FILENAME);
            MPI_exit(3);
        }
        scattered_matrix2D = scatter(entry_matrix2D, numprocs, rank); // Process 0 passes the input matrix to scatter function
        destroy_matrix2D(entry_matrix2D);

        multiply_vector = read_vector_from_file(VECTOR_FILENAME);
        if (multiply_vector == NULL)
        {
            fprintf(stderr, "Error reading vector from file %s\n", VECTOR_FILENAME);
            MPI_exit(4);
        }
    }
    else
    {
        scattered_matrix2D = scatter(NULL, numprocs, rank); // Process 1...n passes NULL to scatter function and receive lines of input matrix
    }

    // In process 0, returns the input vector. On other processes, input vector = NULL, so it read vector passed to the token ring
    multiply_vector = broadcast_multiply_vector(multiply_vector, numprocs, rank);

    // Do multiplication
    Matrix1D *multiplication_result = multiply_Matrix2D_by_vector1D(scattered_matrix2D, multiply_vector);
    destroy_matrix2D(scattered_matrix2D);
    destroy_matrix1D(multiply_vector);
    if (multiplication_result == NULL) // Matrix and vector size don't match
    {
        fprintf(stderr, "Process %d: bad format for matrix or multiply vector\n", rank);
        MPI_exit(2);
    }

    // Returns NULL except on process 0, where it returns the result vector passed by token ring
    Matrix1D *result_matrix = gather(multiplication_result, numprocs, rank);
    destroy_matrix1D(multiplication_result);

    if (rank == 0)
    {
        write_vector_to_file( OUTPUT_VECTOR_FILENAME, result_matrix);
        destroy_matrix1D(result_matrix);
    }

    MPI_Finalize();
    return 0;
}

void MPI_exit(int code)
{
    MPI_Finalize();
    exit(code);
}
