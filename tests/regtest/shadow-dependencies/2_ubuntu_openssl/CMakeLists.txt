## required for independent build
cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)

enable_testing()
## required for independent build end

add_custom_target(openssl-ubuntu-crypto-shared ALL
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/libsslcrypto.so)
add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/libsslcrypto.so
  COMMAND gcc ssl_crypto.c -o libsslcrypto.so -lssl -lcrypto -shared -fPIC
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(openssl-ubuntu-init-shared ALL
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/libsslinit.so)
add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/libsslinit.so
  COMMAND gcc ssl_init.c -o libsslinit.so -lssl -lcrypto -shared -fPIC
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})


add_custom_target(openssl-ubuntu-rand-shared ALL
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/libsslrand.so)
add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/libsslrand.so
  COMMAND gcc ssl_init.c -o libsslrand.so -lssl -lcrypto -shared -fPIC
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

