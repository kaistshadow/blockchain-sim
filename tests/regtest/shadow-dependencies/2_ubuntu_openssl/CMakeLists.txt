## required for independent build
cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)

add_custom_target(curl-ubuntu-crypto-shared ALL
        COMMAND gcc ssl_crypto.c -o libsslcrypto.so -lssl -lcrypto -shared -fPIC
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(curl-ubuntu-init-shared ALL
        COMMAND gcc ssl_init.c -o libsslinit.so -lssl -lcrypto -shared -fPIC
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(curl-ubuntu-rand-shared ALL
        COMMAND g++ ssl_rand.cpp -o libsslrand.so -lssl -lcrypto -shared -fPIC
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME shadow-ubuntu-openssl-libcrypto
        COMMAND shadow ssl_crypto.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME shadow-ubuntu-openssl-libinit
        COMMAND shadow ssl_init.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME shadow-ubuntu-openssl-librand
        COMMAND shadow ssl_rand.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

