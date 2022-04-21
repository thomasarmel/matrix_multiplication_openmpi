#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"
#include "send_recv_matrix.h"

Matrix1D *scatter(Matrix2D *matrix2D, int num_rows, int num_cols, int num_procs, int current_rank);

int main(int argc, char** argv)
{
    int rank, numprocs;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    Matrix1D *matrix1D = NULL;
    if (rank == 0)
    {
        Matrix2D *matrix2D = create_matrix2D(numprocs, numprocs);

        int **arrays = (int **)malloc(sizeof(int *) * numprocs);
        for (int i = 0; i < numprocs; i++)
        {
            arrays[i] = (int *)malloc(sizeof(int) * numprocs);
            for (int j = 0; j < numprocs; j++)
            {
                arrays[i][j] = i * j;
                set_matrix2D_value(matrix2D, i, j, i * j);
            }
        }
        matrix1D = scatter(matrix2D, numprocs, numprocs, numprocs, rank);
        destroy_matrix2D(matrix2D);
    }
    else
    {
        matrix1D = scatter(NULL, numprocs, numprocs, numprocs, rank);
    }
    printf("Process %d: ", rank);
    print_matrix1D(matrix1D);
    MPI_Finalize();
}

Matrix1D *scatter(Matrix2D *matrix2D, int num_rows, int num_cols, int num_procs, int current_rank)
{
    if (current_rank == 0)
    {
        for(int i = num_rows - 1; i >= 1; i--)
        {
            send_matrix2D_line(matrix2D, 1, i);
        }
        return get_matrix2D_line(matrix2D, 0);
    }
    Matrix1D *return_matrix = NULL;
    for(int i = 0; i < (num_procs - current_rank); i++)
    {
        Matrix1D *matrix1D = createMatrix1D(num_cols);
        recv_matrix1D(matrix1D);
        if (i == (num_procs - current_rank) - 1)
        {
            return_matrix = matrix1D;
        }
        else
        {
            send_matrix1D(matrix1D, current_rank + 1);
            destroy_matrix1D(matrix1D);
        }
    }
    return return_matrix;
}