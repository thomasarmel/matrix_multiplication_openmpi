#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include "process_transmissions.h"
#include "matrix_operations.h"
#include "read_input_file.h"

void MPI_exit(int code);

int main(int argc, char **argv)
{
    int rank, numprocs;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

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
    const char *MATRIX_FILENAME = argv[1];
    const char *VECTOR_FILENAME = argv[2];
    if (rank == 0)
    {
        Matrix2D *entry_matrix2D = read_matrix_from_file(MATRIX_FILENAME);
        if (entry_matrix2D == NULL)
        {
            fprintf(stderr, "Error reading matrix from file %s\n", MATRIX_FILENAME);
            MPI_exit(3);
        }
        printf("Matrix:");
        print_matrix2D(entry_matrix2D);
        scattered_matrix2D = scatter(entry_matrix2D, numprocs, rank);
        destroy_matrix2D(entry_matrix2D);

        multiply_vector = read_vector_from_file(VECTOR_FILENAME);
        if (multiply_vector == NULL)
        {
            fprintf(stderr, "Error reading vector from file %s\n", VECTOR_FILENAME);
            MPI_exit(4);
        }
        printf("Multiply vector:");
        print_matrix1D(multiply_vector);
    }
    else
    {
        scattered_matrix2D = scatter(NULL, numprocs, rank);
    }

    multiply_vector = broadcast_multiply_vector(multiply_vector, numprocs, rank);

    Matrix1D *multiplication_result = multiply_Matrix2D_by_vector1D(scattered_matrix2D, multiply_vector);
    destroy_matrix2D(scattered_matrix2D);
    destroy_matrix1D(multiply_vector);
    if (multiplication_result == NULL) // Matrix and vector size don't match
    {
        fprintf(stderr, "Process %d: bad format for matrix or multiply vector\n", rank);
        MPI_exit(2);
    }
    Matrix1D *result_matrix = gather(multiplication_result, numprocs, rank);
    destroy_matrix1D(multiplication_result);

    if (rank == 0)
    {
        printf("Result: ");
        print_matrix1D(result_matrix);
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
