## required for independent build
cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)

enable_testing()
## required for independent build end


add_custom_target(curl-ubuntu-bin ALL
        COMMAND gcc simplecurl.c -o simplecurl -lcurl
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(curl-ubuntu-shared ALL
        COMMAND gcc simplecurl.c -o libsimplecurl.so -lcurl -shared -fPIC
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# system curl test on shadow
# Input : Nothing
# Output : 정상 종료 : return 0 , 아닐 경우 return error
add_test(NAME shadow-ubuntu-libcurl
        COMMAND shadow simplecurl.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

