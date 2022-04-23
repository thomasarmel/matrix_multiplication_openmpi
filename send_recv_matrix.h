#ifndef OPENMPI2_SEND_RECV_MATRIX_H
#define OPENMPI2_SEND_RECV_MATRIX_H

#include "matrix.h"

void send_matrix1D(Matrix1D *matrix, int dest, int flag);
void recv_matrix1D(Matrix1D *matrix, int flag);
void send_matrix2D_line(Matrix2D *matrix, int dest, int line_number, int flag);
void recv_matrix2D_line(Matrix2D *matrix, int line_number, int flag);
void send_dim_1D(int dim, int dest, int flag);
void recv_dim_1D(int *dim, int flag);
void send_dim_2D(int rows, int cols, int dest, int flag);
void recv_dim_2D(int *rows, int *cols, int flag);

#endif //OPENMPI2_SEND_RECV_MATRIX_H
