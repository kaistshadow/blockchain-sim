## required for independent build
cmake_minimum_required(VERSION 2.8...3.13)

## enable CTEST
enable_testing()

# add_subdirectory(1_ubuntu_libcurl) # Commentify since it fails as default
add_subdirectory(2_ubuntu_openssl)
add_subdirectory(3_custom_libcurl)
add_subdirectory(4_custom_openssl)
# add_subdirectory(5_customed_rpc) # Commentify since it leverages bitcoin 0.19.1nw
