#ifndef OPENMPI2_MATRIX_OPERATIONS_H
#define OPENMPI2_MATRIX_OPERATIONS_H

#include "matrix.h"

/**
 * @brief Matrix and vector Multiplication
 * @param matrix
 * @param vector
 * @return Product of matrix and vector
 * @note Matrix number of columns must be equal to vector number of rows
 */
Matrix1D *multiply_Matrix2D_by_vector1D(const Matrix2D *matrix, const Matrix1D *vector);

#endif //OPENMPI2_MATRIX_OPERATIONS_H
