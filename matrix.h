#ifndef OPENMPI2_MATRIX_H
#define OPENMPI2_MATRIX_H

struct matrix1D {
    int *M;
    int N;
};
typedef struct matrix1D Matrix1D;

Matrix1D *create_Matrix1D(int size);
void destroy_matrix1D(Matrix1D *matrix);
int get_matrix1D_value(Matrix1D *matrix, int pos);
void set_matrix1D_value(Matrix1D *matrix, int pos, int value);
void print_matrix1D(Matrix1D *matrix);

struct matrix2D{
    int **tab;
    int rows;
    int columns;
};
typedef struct matrix2D Matrix2D;

Matrix2D *create_matrix2D(int rows, int columns);
void destroy_matrix2D(Matrix2D *matrix);
int get_matrix2D_value(Matrix2D *matrix, int row, int column);
void set_matrix2D_value(Matrix2D *matrix, int row, int column, int value);
void print_matrix2D(Matrix2D *matrix);

Matrix1D *get_matrix2D_line(Matrix2D *matrix, int line);
void set_matrix2D_line(Matrix2D *matrix, int line, const Matrix1D *line_matrix);
Matrix2D *extract_matrix2D_lines(const Matrix2D *matrix, int start_line, int end_line);

#endif //OPENMPI2_MATRIX_H
