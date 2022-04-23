#include <stdlib.h>
#include "process_transmissions.h"
#include "send_recv_matrix.h"
#include "transmission_flags.h"

Matrix1D *broadcast_multiply_vector(Matrix1D *multiply_vector, int num_procs, int current_rank)
{
    if (current_rank == 0)
    {
        // send matrix size
        send_dim_1D(multiply_vector->N, 1, FLAG_MULTIPLY_VECTOR_BROADCAST_SIZE);
        // send matrix data
        send_matrix1D(multiply_vector, 1, FLAG_MULTIPLY_VECTOR_BROADCAST_DATA);
        return multiply_vector;
    }
    else
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

Matrix2D *scatter(const Matrix2D *matrix2D, int num_procs, int current_rank)
{
    int num_rows, num_cols, lines_per_node;
    if (current_rank == 0)
    {
        num_rows = matrix2D->rows;
        lines_per_node = num_rows / num_procs; // How many line of input matrix process should handle
        // send matrix size
        send_dim_2D(num_rows, matrix2D->columns, 1, FLAG_SCATTER_SIZE);
        // send matrix data
        for (int i = num_rows - 1; i >= 1; i--)
        {
            send_matrix2D_line(matrix2D, 1, i, FLAG_SCATTER_DATA);
        }
        // Return lines of the input matrix (note that the data is copied)
        return extract_matrix2D_lines(matrix2D, 0, lines_per_node);
    }
    // receive matrix size
    recv_dim_2D(&num_rows, &num_cols, FLAG_SCATTER_SIZE);
    if (current_rank < num_procs - 1)
    {
        // send matrix size
        send_dim_2D(num_rows, num_cols, current_rank + 1, FLAG_SCATTER_SIZE);
    }
    lines_per_node = num_rows / num_procs; // How many line of input matrix process should handle
    Matrix2D *return_matrix2d = create_matrix2D(lines_per_node, num_cols);
    for (int i = 0; i < (num_procs - current_rank) * lines_per_node; i++) // For each line process should receive
    {
        Matrix1D *received_matrix1D = create_Matrix1D(num_cols);
        recv_matrix1D(received_matrix1D, FLAG_SCATTER_DATA); // Receive matrix line
        const int first_line = (num_procs - current_rank - 1) * lines_per_node;
        if (i >= first_line) // If process should keep received line
        {
            set_matrix2D_line(return_matrix2d, lines_per_node - 1 - (i - first_line), received_matrix1D);
        }
        else // If process should send received line to next process
        {
            send_matrix1D(received_matrix1D, current_rank + 1, FLAG_SCATTER_DATA);
        }
        destroy_matrix1D(received_matrix1D);
    }
    return return_matrix2d;
}

Matrix1D *gather(const Matrix1D *send_matrix, int num_procs, int current_rank)
{
    int lines_per_node = send_matrix->N; // How many line of input matrix process should handle
    int num_rows = lines_per_node * num_procs;
    if (current_rank == 0)
    {
        Matrix1D *return_matrix = create_Matrix1D(num_rows);
        for (int j = 0; j < lines_per_node; j++) // Process 0 already knows this part of vector
        {
            set_matrix1D_value(return_matrix, j, get_matrix1D_value(send_matrix, j));
        }
        for (int i = num_procs - 1; i > 0; i--) // Receive part of vector from higher processes
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
    for (int i = current_rank; i < num_procs - 1; i++) // Last process never inter in this loop
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
    return NULL; // If process != 0, return NULL
}