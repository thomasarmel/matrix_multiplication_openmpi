#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"
#include "send_recv_matrix.h"

Matrix2D *scatter(Matrix2D *matrix2D, int num_rows, int num_cols, int num_procs, int current_rank);

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
    MPI_Finalize();
}

Matrix2D *scatter(Matrix2D *matrix2D, int num_rows, int num_cols, int num_procs, int current_rank)
{
    int lines_per_node = num_rows / num_procs;
    Matrix2D *return_matrix2d = create_matrix2D(lines_per_node, num_cols);
    if (current_rank == 0)
    {
        for(int i = num_rows - 1; i >= 1; i--)
        {
            send_matrix2D_line(matrix2D, 1, i);
        }
        // TODO: optimize
        for(int i = 0; i < lines_per_node; i++)
        {
            Matrix1D *line = get_matrix2D_line(matrix2D, i);
            set_matrix2D_line(return_matrix2d, i, line);
        }
        return return_matrix2d;
    }
    for(int i = 0; i < (num_procs - current_rank) * lines_per_node; i++)
    {
        Matrix1D *matrix1D = createMatrix1D(num_cols);
        recv_matrix1D(matrix1D);
        const int first_line = (num_procs - current_rank - 1) * lines_per_node;
        if (i >= first_line)
        {
            set_matrix2D_line(return_matrix2d, lines_per_node - 1 - (i - first_line), matrix1D);
        }
        else
        {
            send_matrix1D(matrix1D, current_rank + 1);
            destroy_matrix1D(matrix1D);
        }
    }
    return return_matrix2d;
}