#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include "process_transmissions.h"
#include "matrix_operations.h"
#include "read_write_file.h"

void MPI_exit(int code);

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
