#ifndef OPENMPI2_SEND_RECV_MATRIX_H
#define OPENMPI2_SEND_RECV_MATRIX_H

#include "matrix.h"

void send_matrix1D(Matrix1D *matrix, int dest);
void recv_matrix1D(Matrix1D *matrix);
void send_matrix2D_line(Matrix2D *matrix, int dest, int line_number);
void recv_matrix2D_line(Matrix2D *matrix, int line_number);

#endif //OPENMPI2_SEND_RECV_MATRIX_H
