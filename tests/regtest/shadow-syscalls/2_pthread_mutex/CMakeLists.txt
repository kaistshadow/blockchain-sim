cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)
enable_testing()

add_custom_target(glibc-pthread ALL
        COMMAND gcc -o pthread.so pthread.c -lpthread -shared -fPIC -g
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# pthread test
# 설명 : pthread mutex관련 함수 shadow 테스트
# Input : Nothing
# Output : 정상 종료 시 return PASS 아닐 경우 return FAIL
add_test(NAME shadow-glibc-pthread
        COMMAND shadow example_pthread.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})