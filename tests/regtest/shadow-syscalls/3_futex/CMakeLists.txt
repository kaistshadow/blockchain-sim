cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)
enable_testing()

add_custom_target(future ALL
        COMMAND g++ -o future.so future.c -lpthread -shared -fPIC -g
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# futex test
# 설명 : shadow환경에서 futex의 간단 사용 예제 테스트
# Input : Nothing
# Output : 정상 종료 시 return 0, 아닐 경우 return -1
add_test(NAME shadow-futex-future
        COMMAND shadow example_future.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set_tests_properties(shadow-futex-future PROPERTIES TIMEOUT 30)