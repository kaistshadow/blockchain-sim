## required for independent build
cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)

## enable CTEST
enable_testing()

add_subdirectory(1_eventfd)
add_subdirectory(2_pthread_mutex)
