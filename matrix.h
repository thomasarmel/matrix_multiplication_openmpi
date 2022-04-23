#ifndef OPENMPI2_MATRIX_H
#define OPENMPI2_MATRIX_H

struct matrix1D {
    int *M;
    int N;
};
typedef struct matrix1D Matrix1D;

Matrix1D *create_Matrix1D(int size); // constructor
void destroy_matrix1D(Matrix1D *matrix); // destructor
int get_matrix1D_value(const Matrix1D *matrix, int pos); // getter
void set_matrix1D_value(Matrix1D *matrix, int pos, int value); // setter
void print_matrix1D(Matrix1D *matrix);

struct matrix2D{
    int **tab;
    int rows;
    int columns;
};
typedef struct matrix2D Matrix2D;

Matrix2D *create_matrix2D(int rows, int columns); // constructor
void destroy_matrix2D(Matrix2D *matrix); // destructor
int get_matrix2D_value(const Matrix2D *matrix, int row, int column); // getter
void set_matrix2D_value(Matrix2D *matrix, int row, int column, int value); // setter
void print_matrix2D(Matrix2D *matrix);

Matrix1D *get_matrix2D_line(const Matrix2D *matrix, int line); // Extract a line from a 2D matrix
void set_matrix2D_line(Matrix2D *matrix, int line, const Matrix1D *line_matrix); // Set 2D matrix line from 1D matrix
Matrix2D *extract_matrix2D_lines(const Matrix2D *matrix, int start_line, int end_line); // Extract 2D matrix lines

#endif //OPENMPI2_MATRIX_H
