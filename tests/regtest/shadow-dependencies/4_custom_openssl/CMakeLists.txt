## required for independent build
cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)

## enable CTEST
enable_testing()


include(ExternalProject)

# SOURCE_DIR = <prefix>/src/<name>
# BINARY_DIR = <prefix>/src/<name>-build
# INSTALL_DIR = <prefix>
ExternalProject_Add(openssl
        URL https://www.openssl.org/source/openssl-1.1.0h.tar.gz
        CONFIGURE_COMMAND <SOURCE_DIR>/./config --prefix=${CMAKE_CURRENT_BINARY_DIR} shared threads enable-ec_nistp_64_gcc_128 -fPIC
        BUILD_COMMAND make depend
        COMMAND make -j 6
        COMMAND make install_sw
        COMMAND make clean
        BUILD_IN_SOURCE TRUE
        INSTALL_COMMAND ""
        )
#
#ExternalProject_Add(curl
#        URL https://curl.haxx.se/download/curl-7.70.0.tar.gz
#        BUILD_COMMAND make
#        COMMAND ./buildconf
#        CONFIGURE_COMMAND <SOURCE_DIR>/./configure --prefix=${CMAKE_CURRENT_BINARY_DIR} --with-ssl=${CMAKE_CURRENT_BINARY_DIR}
#        COMMAND make -j 6
#        COMMAND make install
#        COMMAND make clean
#        BUILD_IN_SOURCE TRUE
#        INSTALL_COMMAND ""
#        )

add_custom_target(openssl-custom-shared ALL
        COMMAND gcc ssl_rand.cpp -o libsslrand.so -L${CMAKE_CURRENT_BINARY_DIR}/lib -lssl -lcrypto -shared -fPIC -Wl,-rpath=${CMAKE_CURRENT_BINARY_DIR}/lib
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME shadow-custom-openssl-librand
        COMMAND shadow ssl_rand.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

