## required for independent build
cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)

## enable CTEST
enable_testing()

add_subdirectory(1_ubuntu_libcurl)
add_subdirectory(2_ubuntu_openssl)
add_subdirectory(3_custom_libcurl)
add_subdirectory(4_custom_openssl)
add_subdirectory(6_p11-kit)
add_subdirectory(8_customed_rpc)
add_subdirectory(9_wallet)
