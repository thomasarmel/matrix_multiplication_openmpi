#include <stdlib.h>
#include "matrix_operations.h"

Matrix1D *multiply_Matrix2D_by_vector1D(Matrix2D *matrix, Matrix1D *vector)
{
    if (vector->N != matrix->columns)
    {
        return NULL;
    }
    Matrix1D *result = create_Matrix1D(matrix->rows);
    for (int i = 0; i < matrix->rows; i++)
    {
        set_matrix1D_value(result, i, 0);
        for (int j = 0; j < matrix->columns; j++)
        {
            int new_value = get_matrix1D_value(result, i) + (get_matrix2D_value(matrix, i, j) * get_matrix1D_value(vector, j));
            set_matrix1D_value(result, i, new_value);
        }
    }
    return result;
}