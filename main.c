#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"
#include "send_recv_matrix.h"

Matrix2D *scatter(Matrix2D *matrix2D, int num_rows, int num_cols, int num_procs, int current_rank);
Matrix1D *broadcast_multiply_vector(Matrix1D *multiply_vector, int num_procs, int current_rank);

int main(int argc, char** argv)
{
    int rank, numprocs;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    Matrix2D *scattered_matrix2D = NULL;
    const int MATRIX_COLS = 6;
    const int MATRIX_ROWS = numprocs * 3;
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
        scattered_matrix2D = scatter(matrix2D, MATRIX_ROWS, MATRIX_COLS, numprocs, rank);
        destroy_matrix2D(matrix2D);
    }
    else
    {
        scattered_matrix2D = scatter(NULL, MATRIX_ROWS, MATRIX_COLS, numprocs, rank);
    }
    printf("Process %d: ", rank);
    print_matrix2D(scattered_matrix2D);

    Matrix1D *multiply_vector = NULL;
    if (rank == 0)
    {
        multiply_vector = create_Matrix1D(7);
        for (int i = 0; i < 7; i++)
        {
            set_matrix1D_value(multiply_vector, i, i);
        }
    }
    multiply_vector = broadcast_multiply_vector(multiply_vector, numprocs, rank);
    printf("multiply Process %d: ", rank);
    print_matrix1D(multiply_vector);

    MPI_Finalize();
}

Matrix1D *broadcast_multiply_vector(Matrix1D *multiply_vector, int num_procs, int current_rank)
{
    const int VECTOR_SIZE_FLAG = 1;
    const int VECTOR_DATA_FLAG = 2;
    if(current_rank == 0)
    {
        // send matrix size
        MPI_Send(&multiply_vector->N, 1, MPI_INT, 1, VECTOR_SIZE_FLAG, MPI_COMM_WORLD);
        // send matrix data
        send_matrix1D(multiply_vector, 1, VECTOR_DATA_FLAG);
        return multiply_vector;
    }
    else
    {
        // receive matrix size
        int size;
        MPI_Recv(&size, 1, MPI_INT, current_rank - 1, VECTOR_SIZE_FLAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if (current_rank < num_procs - 1)
        {
            // send matrix size
            MPI_Send(&size, 1, MPI_INT, current_rank + 1, VECTOR_SIZE_FLAG, MPI_COMM_WORLD);
        }
        // receive matrix data
        Matrix1D *received_vector = create_Matrix1D(size);
        recv_matrix1D(received_vector, VECTOR_DATA_FLAG);
        if (current_rank < num_procs - 1)
        {
            // send matrix data
            send_matrix1D(received_vector, current_rank + 1, VECTOR_DATA_FLAG);
        }
        return received_vector;
    }
}

Matrix2D *scatter(Matrix2D *matrix2D, int num_rows, int num_cols, int num_procs, int current_rank)
{
    int lines_per_node = num_rows / num_procs;
    if (current_rank == 0)
    {
        for(int i = num_rows - 1; i >= 1; i--)
        {
            send_matrix2D_line(matrix2D, 1, i);
        }
        return extract_matrix2D_lines(matrix2D, 0, lines_per_node);
    }
    Matrix2D *return_matrix2d = create_matrix2D(lines_per_node, num_cols);
    for(int i = 0; i < (num_procs - current_rank) * lines_per_node; i++)
    {
        Matrix1D *matrix1D = create_Matrix1D(num_cols);
        recv_matrix1D(matrix1D, 0);
        const int first_line = (num_procs - current_rank - 1) * lines_per_node;
        if (i >= first_line)
        {
            set_matrix2D_line(return_matrix2d, lines_per_node - 1 - (i - first_line), matrix1D);
        }
        else
        {
            send_matrix1D(matrix1D, current_rank + 1, 0);
            destroy_matrix1D(matrix1D);
        }
    }
    return return_matrix2d;
}