#include <mpi.h>
#include "send_recv_matrix.h"

void send_matrix1D(Matrix1D *matrix, int dest, int flag)
{
    MPI_Send(matrix->M, matrix->N * sizeof(int), MPI_BYTE, dest, flag, MPI_COMM_WORLD);
}

void recv_matrix1D(Matrix1D *matrix, int flag)
{
    MPI_Recv(matrix->M, matrix->N * sizeof(int), MPI_BYTE, MPI_ANY_SOURCE, flag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

void send_matrix2D_line(Matrix2D *matrix, int dest, int line_number)
{
    MPI_Send(matrix->tab[line_number], matrix->columns * sizeof(int), MPI_BYTE, dest, 0, MPI_COMM_WORLD);
}

void recv_matrix2D_line(Matrix2D *matrix, int line_number)
{
    MPI_Recv(matrix->tab[line_number], matrix->columns * sizeof(int), MPI_BYTE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}