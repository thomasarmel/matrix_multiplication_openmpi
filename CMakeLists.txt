cmake_minimum_required(VERSION 3.16)
project(openmpi2)

find_package(MPI REQUIRED)

include_directories(${MPI_INCLUDE_PATH})

set(CMAKE_CXX_STANDARD 14)

add_executable(openmpi2 main.c matrix.h matric.c send_recv_matrix.h send_recv_matrix.c transmission_flags.h)

target_link_libraries(openmpi2 ${MPI_LIBRARIES})