## required for independent build
cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)
## enable CTEST
enable_testing()
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake")
find_package(EOS_2_0_7 REQUIRED)
#add_custom_target(shadow-eos-prepare ALL
#        COMMAND mkdir -p data/node1
#        COMMAND mkdir -p data/node2
#        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(copy-eos-dev-plugin ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Eosio_2_0_7::Lib> ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(copy-eos-bin ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Eosio_2_0_7::Bin> ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME shadow-eos-regtest
        COMMAND shadow example.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})


