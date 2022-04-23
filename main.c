#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"
#include "send_recv_matrix.h"
#include "transmission_flags.h"
#include "matrix_operations.h"

Matrix2D *scatter(Matrix2D *matrix2D, int num_procs, int current_rank);

Matrix1D *gather(const Matrix1D *send_matrix, int num_procs, int current_rank);

Matrix1D *broadcast_multiply_vector(Matrix1D *multiply_vector, int num_procs, int current_rank);

int main(int argc, char **argv)
{
    int rank, numprocs;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    Matrix2D *scattered_matrix2D = NULL;
    const int MATRIX_COLS = 6;
    const int MATRIX_ROWS = numprocs * 3;
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
        scattered_matrix2D = scatter(matrix2D, numprocs, rank);
        destroy_matrix2D(matrix2D);
    } else
    {
        scattered_matrix2D = scatter(NULL, numprocs, rank);
    }
    printf("Process %d: ", rank);
    print_matrix2D(scattered_matrix2D);

    Matrix1D *multiply_vector = NULL;
    if (rank == 0)
    {
        multiply_vector = create_Matrix1D(MULTIPLY_VECTOR_SIZE);
        for (int i = 0; i < MULTIPLY_VECTOR_SIZE; i++)
        {
            set_matrix1D_value(multiply_vector, i, i);
        }
        Matrix1D *multiplication_result = multiply_Matrix2D_by_vector1D(scattered_matrix2D, multiply_vector);
        printf("Multiplication result for process 0: ");
        print_matrix1D(multiplication_result);
        destroy_matrix1D(multiplication_result);
    }
    multiply_vector = broadcast_multiply_vector(multiply_vector, numprocs, rank);
    printf("multiply Process %d: ", rank);
    print_matrix1D(multiply_vector);

    for (int i = 0; i < multiply_vector->N; i++)
    {
        set_matrix1D_value(multiply_vector, i, get_matrix1D_value(multiply_vector, i) + rank);
    }

    Matrix1D *result_vector = gather(multiply_vector, numprocs, rank);
    if (rank == 0)
    {
        printf("result Process %d: ", rank);
        print_matrix1D(result_vector);
        destroy_matrix1D(result_vector);
    }

    MPI_Finalize();
}

Matrix1D *broadcast_multiply_vector(Matrix1D *multiply_vector, int num_procs, int current_rank)
{
    if (current_rank == 0)
    {
        // send matrix size
        send_dim_1D(multiply_vector->N, 1, FLAG_MULTIPLY_VECTOR_BROADCAST_SIZE);
        // send matrix data
        send_matrix1D(multiply_vector, 1, FLAG_MULTIPLY_VECTOR_BROADCAST_DATA);
        return multiply_vector;
    } else
    {
        // receive matrix size
        int size;
        recv_dim_1D(&size, FLAG_MULTIPLY_VECTOR_BROADCAST_SIZE);
        if (current_rank < num_procs - 1)
        {
            // send matrix size
            send_dim_1D(size, current_rank + 1, FLAG_MULTIPLY_VECTOR_BROADCAST_SIZE);
        }
        // receive matrix data
        Matrix1D *received_vector = create_Matrix1D(size);
        recv_matrix1D(received_vector, FLAG_MULTIPLY_VECTOR_BROADCAST_DATA);
        if (current_rank < num_procs - 1)
        {
            // send matrix data
            send_matrix1D(received_vector, current_rank + 1, FLAG_MULTIPLY_VECTOR_BROADCAST_DATA);
        }
        return received_vector;
    }
}

Matrix2D *scatter(Matrix2D *matrix2D, int num_procs, int current_rank)
{
    int num_rows, num_cols, lines_per_node;
    if (current_rank == 0)
    {
        num_rows = matrix2D->rows;
        lines_per_node = num_rows / num_procs;
        // send matrix size
        send_dim_2D(num_rows, matrix2D->columns, 1, FLAG_SCATTER_SIZE);
        for (int i = num_rows - 1; i >= 1; i--)
        {
            send_matrix2D_line(matrix2D, 1, i, FLAG_SCATTER_DATA);
        }
        return extract_matrix2D_lines(matrix2D, 0, lines_per_node);
    }
    // receive matrix size
    recv_dim_2D(&num_rows, &num_cols, FLAG_SCATTER_SIZE);
    if (current_rank < num_procs - 1)
    {
        // send matrix size
        send_dim_2D(num_rows, num_cols, current_rank + 1, FLAG_SCATTER_SIZE);
    }
    lines_per_node = num_rows / num_procs;
    Matrix2D *return_matrix2d = create_matrix2D(lines_per_node, num_cols);
    for (int i = 0; i < (num_procs - current_rank) * lines_per_node; i++)
    {
        Matrix1D *matrix1D = create_Matrix1D(num_cols);
        recv_matrix1D(matrix1D, FLAG_SCATTER_DATA);
        const int first_line = (num_procs - current_rank - 1) * lines_per_node;
        if (i >= first_line)
        {
            set_matrix2D_line(return_matrix2d, lines_per_node - 1 - (i - first_line), matrix1D);
        } else
        {
            send_matrix1D(matrix1D, current_rank + 1, FLAG_SCATTER_DATA);
            destroy_matrix1D(matrix1D);
        }
    }
    return return_matrix2d;
}

Matrix1D *gather(const Matrix1D *send_matrix, int num_procs, int current_rank)
{
    int lines_per_node = send_matrix->N;
    int num_rows = lines_per_node * num_procs;
    if (current_rank == 0)
    {
        Matrix1D *return_matrix = create_Matrix1D(num_rows);
        for (int j = 0; j < lines_per_node; j++)
        {
            set_matrix1D_value(return_matrix, j, get_matrix1D_value(send_matrix, j));
        }
        for (int i = num_procs - 1; i > 0; i--)
        {
            Matrix1D *received_matrix = create_Matrix1D(lines_per_node);
            recv_matrix1D(received_matrix, FLAG_GATHER_DATA);
            for (int j = 0; j < lines_per_node; j++)
            {
                set_matrix1D_value(return_matrix, (i * lines_per_node) + j, get_matrix1D_value(received_matrix, j));
            }
            destroy_matrix1D(received_matrix);
        }
        return return_matrix;
    }
    for (int i = current_rank; i < num_procs - 1; i++)
    {
        // receive matrix data from rank n + 1
        Matrix1D *received_matrix = create_Matrix1D(lines_per_node);
        recv_matrix1D(received_matrix, FLAG_GATHER_DATA);
        // send received matrix data to rank n - 1
        send_matrix1D(received_matrix, current_rank - 1, FLAG_GATHER_DATA);
        destroy_matrix1D(received_matrix);
    }
    // send current matrix data to rank n - 1
    send_matrix1D(send_matrix, current_rank - 1, FLAG_GATHER_DATA);
    return NULL;
}