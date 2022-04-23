#include <stdio.h>
#include "read_write_file.h"

Matrix2D *read_matrix_from_file(const char *filename)
{
    // Open file
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        return NULL;
    }

    // Count rows and columns
    int rows = 0;
    int columns = 0;
    int current_columns = 0; // To check if all rows have the same number of columns
    int is_first_line = 1;
    int c;
    int previous_c = 0;
    while ((c = fgetc(file)) != EOF)
    {
        if (c == '\r') // Ignore carriage return
        {
            continue;
        }
        if (c == '\n')
        {
            if (previous_c == '\n') // End of matrix
            {
                break;
            }
            if (previous_c != ' ') // Handle missinng whitespace at the end of the line
            {
                if (is_first_line)
                {
                    columns++;
                }
                current_columns++;
            }
            rows++;
            is_first_line = 0;
            if (current_columns != columns) // Bad file format, all rows must have the same number of columns
            {
                fclose(file);
                return NULL;
            }
            current_columns = 0;
        }
        else if (c == ' ' && previous_c != ' ')
        {
            if (is_first_line)
            {
                columns++;
            }
            current_columns++;
        }
        previous_c = c;
    }
    if (previous_c != '\n')
    {
        rows++;
    }
    if (columns == 0 || rows == 0)
    {
        fclose(file);
        return NULL;
    }
    rewind(file); // Beginning of file
    // Allocate memory for matrix
    Matrix2D *return_matrix = create_matrix2D(rows, columns);

    // Read matrix with scanf
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            fscanf(file, "%d", &return_matrix->tab[i][j]);
        }
    }
    fclose(file);
    return return_matrix;
}

Matrix1D *read_vector_from_file(const char *filename)
{
    // Read file, 1 integer per line
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        return NULL;
    }
    int rows = 0;
    int c;
    int previous_c = 0;
    while ((c = fgetc(file)) != EOF)
    {
        if (c == '\r') // Ignore carriage return
        {
            continue;
        }
        if (c == '\n')
        {
            if (previous_c == '\n')
            {
                break;
            }
            rows++;
        }
        previous_c = c;
    }
    if (previous_c != '\n')
    {
        rows++;
    }
    rewind(file); // Beginning of file
    Matrix1D *return_vector = create_Matrix1D(rows);
    for (int i = 0; i < rows; i++)
    {
        fscanf(file, "%d", &return_vector->M[i]);
    }
    fclose(file);
    return return_vector;
}

int write_vector_to_file(const char *filename, Matrix1D *vector)
{
    // Open file
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        return -1;
    }
    // Write vector to file
    for (int i = 0; i < vector->N; i++)
    {
        fprintf(file, "%d \n", vector->M[i]);
    }
    fclose(file);
    return 0;
}