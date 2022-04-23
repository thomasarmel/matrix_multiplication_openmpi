#ifndef OPENMPI2_PROCESS_TRANSMISSIONS_H
#define OPENMPI2_PROCESS_TRANSMISSIONS_H

#include "matrix.h"

Matrix2D *scatter(Matrix2D *matrix2D, int num_procs, int current_rank);

Matrix1D *gather(const Matrix1D *send_matrix, int num_procs, int current_rank);

Matrix1D *broadcast_multiply_vector(Matrix1D *multiply_vector, int num_procs, int current_rank);

#endif //OPENMPI2_PROCESS_TRANSMISSIONS_H
