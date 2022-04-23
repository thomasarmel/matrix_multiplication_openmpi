#ifndef OPENMPI2_READ_INPUT_FILE_H
#define OPENMPI2_READ_INPUT_FILE_H

#include "matrix.h"

Matrix2D *read_matrix_from_file(const char *filename);
Matrix1D *read_vector_from_file(const char *filename);

#endif //OPENMPI2_READ_INPUT_FILE_H
