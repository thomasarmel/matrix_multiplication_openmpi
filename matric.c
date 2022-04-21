#include <stdlib.h>
#include <stdio.h>
#include "matrix.h"

Matrix2D *create_matrix2D(int rows, int columns)
{
    Matrix2D *matrix = (Matrix2D *)malloc(sizeof(Matrix2D));
    matrix->rows = rows;
    matrix->columns = columns;
    matrix->tab = (int**)malloc(rows * sizeof(int*));
    for (int i = 0; i < rows; i++)
    {
        matrix->tab[i] = (int*)malloc(columns * sizeof(int));
    }
    return matrix;
}

void destroy_matrix2D(Matrix2D *matrix)
{
    for (int i = 0; i < matrix->rows; i++)
    {
        free(matrix->tab[i]);
    }
    free(matrix->tab);
    free(matrix);
}

int get_matrix2D_value(Matrix2D *matrix, int row, int column)
{
    return matrix->tab[row][column];
}

void set_matrix2D_value(Matrix2D *matrix, int row, int column, int value)
{
    matrix->tab[row][column] = value;
}

void print_matrix2D(Matrix2D *matrix)
{
    for (int i = 0; i < matrix->rows; i++)
    {
        for (int j = 0; j < matrix->columns; j++)
        {
            printf("%d ", matrix->tab[i][j]);
        }
        printf("//");
    }
    printf("\n");
}

Matrix1D *createMatrix1D(int size)
{
    Matrix1D *matrix = (Matrix1D *)malloc(sizeof(Matrix1D));
    matrix->N = size;
    matrix->M = (int*)malloc(size * sizeof(int));
    return matrix;
}

void destroy_matrix1D(Matrix1D *matrix)
{
    free(matrix->M);
    free(matrix);
}

int get_matrix1D_value(Matrix1D *matrix, int pos)
{
    return matrix->M[pos];
}

void set_matrix1D_value(Matrix1D *matrix, int pos, int value)
{
    matrix->M[pos] = value;
}

void print_matrix1D(Matrix1D *matrix)
{
    printf("(");
    for (int i = 0; i < matrix->N; i++)
    {
        printf("%d ", matrix->M[i]);
    }
    printf(")\n");
}

Matrix1D *get_matrix2D_line(Matrix2D *matrix, int line)
{
    Matrix1D *matrix1D = createMatrix1D(matrix->columns);
    for (int i = 0; i < matrix->columns; i++)
    {
        matrix1D->M[i] = matrix->tab[line][i];
    }
    return matrix1D;
}

void set_matrix2D_line(Matrix2D *matrix, int line, const Matrix1D *line_matrix)
{
    for (int i = 0; i < matrix->columns; i++)
    {
        matrix->tab[line][i] = line_matrix->M[i];
    }
}

Matrix2D *extract_matrix2D_lines(const Matrix2D *matrix, int start_line, int end_line)
{
    Matrix2D *matrix2D = create_matrix2D(end_line - start_line, matrix->columns);
    for (int i = start_line; i < end_line; i++)
    {
        for(int j = 0; j < matrix->columns; j++)
        {
            matrix2D->tab[i - start_line][j] = matrix->tab[i][j];
        }
    }
    return matrix2D;
}