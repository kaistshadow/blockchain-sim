cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)
enable_testing()

add_subdirectory(libevent2.1-6)

# copy libBITCOIND
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../../shadow-bitcoin/1_firstrun/libBITCOIND.so
        DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/)
        
# "3_custom_libcurl" must be installed first.
add_custom_target(bitcoin-dir-prepare ALL
        COMMAND mkdir node1
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(bitcoin-rpc ALL
        COMMAND gcc -o rpc.so rpc.c -lcurl -lssl -shared -fPIC -Wl,-rpath=/${CMAKE_CURRENT_SOURCE_DIR}/../build/3_custom_libcurl/lib -L/${CMAKE_CURRENT_SOURCE_DIR}/../build/3_custom_libcurl/lib -g
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(bitcoin-rest ALL
        COMMAND gcc -o rest.so rest.c -shared -fPIC -g
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME shadow-bitcoin-rpc
        COMMAND shadow example.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(1_pthread)
