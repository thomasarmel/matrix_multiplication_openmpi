cmake_minimum_required(VERSION 3.16)
project(matmult)

find_package(MPI REQUIRED)

include_directories(${MPI_INCLUDE_PATH})

set(CMAKE_CXX_STANDARD 14)

IF (NOT WIN32)
    set(CMAKE_C_COMPILER mpicc)
endif()

add_executable(matmult main.c matrix.h matric.c send_recv_matrix.h send_recv_matrix.c transmission_flags.h matrix_operations.h matrix_operations.c process_transmissions.h process_transmissions.c read_input_file.h read_input_file.c)

target_link_libraries(matmult ${MPI_LIBRARIES})