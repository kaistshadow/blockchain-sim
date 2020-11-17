cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)

if (NOT CMAKE_INSTALL_PREFIX OR
    CMAKE_INSTALL_PREFIX STREQUAL "/usr/local")
  set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/../../../Install")
endif()

set(EOSIO_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/eos")
set(EOSIO_INSTALL_LOCATION "${EOSIO_LOCATION}/../install")
set(EOSIO_BUILD_LOCATION "${EOSIO_LOCATION}/build")

file(GLOB_RECURSE EOS_2_0_7_SRCS ${CMAKE_CURRENT_SOURCE_DIR} *.c *.h *.cpp)

add_custom_target(eosio_2.0.7 ALL
  DEPENDS ${CMAKE_INSTALL_PREFIX}/plugins/libNODEOS_2.0.7.so
  )

add_custom_command(OUTPUT ${CMAKE_INSTALL_PREFIX}/plugins/libNODEOS_2.0.7.so
#  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../tests/regtest/shadow-eos/bin/libNODEOS_2.0.7.so
#  DEPENDS openssl_1.1.0h
#  COMMAND mkdir -p ${EOSIO_INSTALL_LOCATION}
#  COMMAND git clone -b v2.0.7 https://github.com/kaistshadow/eos.git ${EOSIO_LOCATION}
  DEPENDS ${EOS_2_0_7_SRCS}
  COMMAND cd ${EOSIO_LOCATION} && git submodule update --init --recursive
  COMMAND mkdir -p ${EOSIO_BUILD_LOCATION}
  COMMAND cd ${EOSIO_BUILD_LOCATION} && cmake -DCMAKE_BUILD_TYPE='Debug' -DCMAKE_VERBOSE_MAKEFILE=ON -DCMAKE_CXX_COMPILER='clang++-7' -DCMAKE_C_COMPILER='clang-7' -DLLVM_DIR='/usr/lib/llvm-7/lib/cmake/llvm' -DCMAKE_INSTALL_PREFIX=${EOSIO_INSTALL_LOCATION} -DBUILD_MONGO_DB_PLUGIN=true ${EOSIO_LOCATION}
  COMMAND cd ${EOSIO_BUILD_LOCATION} && make -j$(nproc)
#  COMMAND ${CMAKE_COMMAND} -E copy nodeos ${CMAKE_INSTALL_PREFIX}/../tests/libBITCOIND_0.19.1nw.so
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  )

# Define targets that can be used in same build-tree
#add_library(Bitcoin_0_19_1nw::Plugin SHARED IMPORTED GLOBAL)
#add_dependencies(Bitcoin_0_19_1nw::Plugin bitcoin_0.19.1nw)
#set_target_properties(Bitcoin_0_19_1nw::Plugin PROPERTIES
#  IMPORTED_LINK_INTERFACE_LANGUAGES "C"
#  IMPORTED_LOCATION ${CMAKE_INSTALL_PREFIX}/plugins/libBITCOIND_0.19.1nw.so)