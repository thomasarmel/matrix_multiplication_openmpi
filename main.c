#include <stdio.h>
#include <mpi.h>
#include "process_transmissions.h"
#include "matrix_operations.h"

int main(int argc, char **argv)
{
    int rank, numprocs;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    Matrix2D *scattered_matrix2D = NULL;
    const int MATRIX_COLS = 6;
    const int MATRIX_ROWS = numprocs * 1;
    const int MULTIPLY_VECTOR_SIZE = 6;
    if (rank == 0)
    {
        Matrix2D *matrix2D = create_matrix2D(MATRIX_ROWS, MATRIX_COLS);
        int value = 0;
        for (int i = 0; i < MATRIX_ROWS; i++)
        {
            for (int j = 0; j < MATRIX_COLS; j++)
            {
                set_matrix2D_value(matrix2D, i, j, value++);
            }
        }
        printf("Matrix:");
        print_matrix2D(matrix2D);
        scattered_matrix2D = scatter(matrix2D, numprocs, rank);
        destroy_matrix2D(matrix2D);
    } else
    {
        scattered_matrix2D = scatter(NULL, numprocs, rank);
    }

    Matrix1D *multiply_vector = NULL;
    if (rank == 0)
    {
        multiply_vector = create_Matrix1D(MULTIPLY_VECTOR_SIZE);
        for (int i = 0; i < MULTIPLY_VECTOR_SIZE; i++)
        {
            set_matrix1D_value(multiply_vector, i, i);
        }
        printf("Multiply vector:");
        print_matrix1D(multiply_vector);
    }
    multiply_vector = broadcast_multiply_vector(multiply_vector, numprocs, rank);

    Matrix1D *multiplication_result = multiply_Matrix2D_by_vector1D(scattered_matrix2D, multiply_vector);
    if (multiplication_result == NULL) // Matrix and vector size don't match
    {
        fprintf(stderr, "Bad format for matrix or multiply vector\n");
        return -1;
    }
    Matrix1D *result_matrix = gather(multiplication_result, numprocs, rank);
    destroy_matrix1D(multiplication_result);

    if (rank == 0)
    {
        printf("result Process %d: ", rank);
        print_matrix1D(result_matrix);
        destroy_matrix1D(result_matrix);
    }

    MPI_Finalize();
}