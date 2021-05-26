if (ALL_OPT STREQUAL ON)
    add_subdirectory(datadirDump)
elseif (GIT_OPT STREQUAL ON)
    add_subdirectory(datadirDump)
elseif (BITCOIN_OPT STREQUAL ON)
    add_subdirectory(datadirDump)
elseif (MONERO_OPT STREQUAL ON)

else (ALL_OPT STREQUAL ON)
    add_subdirectory(datadirDump)
endif (ALL_OPT STREQUAL ON)

add_library(RPC_LIB SHARED rpc_client.cpp )
target_link_libraries(RPC_LIB CURL_7_70_0::CURL)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3")


install(TARGETS RPC_LIB EXPORT FindBLEEP DESTINATION lib) # Why export here? : https://stackoverflow.com/a/5379238

target_include_directories(RPC_LIB
  PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

install(FILES rpc_client.h 
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})