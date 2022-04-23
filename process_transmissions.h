#ifndef OPENMPI2_PROCESS_TRANSMISSIONS_H
#define OPENMPI2_PROCESS_TRANSMISSIONS_H

#include "matrix.h"

/**
 * @brief Processes the transmission of a matrix on a token ring, line by line.
 * @param matrix2D Input matrix, should be NULL if process != 0
 * @param num_procs Number of processes
 * @param current_rank Rank of the current process
 * @return A 2D matrix representing the matrix lines received by the current process on the token ring
 * @note Suppose we have a 8 x 8 matrix, and 4 processes. Process 0 will keep the first 2 lines, and send the 6 others to process 1.
 * Then process 1 will keep the second 2 lines, and send the 4 others to process 2.
 * Finally for a A x B matrix, the transmission cost will be O(B * A * num_processes ^ 2) - Gauss sum.
 */
Matrix2D *scatter(const Matrix2D *matrix2D, int num_procs, int current_rank);

/**
 * @brief Collect vectors from all process to process 0, on the token ring.
 * @param send_matrix Vector to send to process 0
 * @param num_procs Number of processes
 * @param current_rank Rank of the current process
 * @return Vector collected from the token ring if process = 0, NULL otherwise
 * @note The whole vector is passed directly to the token ring. If size(send_matrix) = N, the transmission cost will be O(N * num_processes ^ 2) - Gauss sum.
 */
Matrix1D *gather(const Matrix1D *send_matrix, int num_procs, int current_rank);

/**
 * @brief Processes the broadcast of the multiply vector from process 0 to all other processes, on the token ring.
 * @param multiply_vector Vector to broadcast to all processes
 * @param num_procs Number of processes
 * @param current_rank Rank of the current process
 * @return Received vector from the token ring. Note that is process = 0, the returned pointer is the same as the input vector.
 * @note The whole vector is passed directly to the token ring. If size(multiply_vector) = N, the transmission cost will be O(N * num_processes ^ 2) - Gauss sum.
 */
Matrix1D *broadcast_multiply_vector(Matrix1D *multiply_vector, int num_procs, int current_rank);

#endif //OPENMPI2_PROCESS_TRANSMISSIONS_H
