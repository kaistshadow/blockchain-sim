## required for independent build
cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)

enable_testing()
## required for independent build end

add_custom_target(openssl-ubuntu-crypto-shared ALL
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/libsslcrypto.so)
add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/libsslcrypto.so
  COMMAND gcc ssl_crypto.c -o libsslcrypto.so -lssl -lcrypto -shared -fPIC
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# system openssl test on shadow
# 주어진 키 값을 system openssl의 라이브러리를 활용하여 암호화, 복호화 하는 테스트
# Input : Nothing
# Output : 정상 종료 시 return 0, 아닐 경우 return -1
add_test(NAME shadow-ubuntu-openssl-libcrypto
        COMMAND shadow ssl_crypto.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
