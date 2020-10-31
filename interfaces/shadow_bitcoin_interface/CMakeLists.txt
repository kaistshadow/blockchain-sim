add_library(SHADOW_BITCOIN_INTERFACE SHARED shadow_bitcoin_interface.cpp)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3")
add_dependencies(SHADOW_BITCOIN_INTERFACE SHADOW_INTERFACE)
target_link_libraries(SHADOW_BITCOIN_INTERFACE SHADOW_INTERFACE)
install(TARGETS SHADOW_BITCOIN_INTERFACE EXPORT FindBLEEP DESTINATION lib) # Why export here? : https://stackoverflow.com/a/5379238

target_include_directories(SHADOW_BITCOIN_INTERFACE
  PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

include_directories(${CMAKE_ROOT_SOURCE_DIR}/interfaces/shadow_interface)

install(FILES shadow_bitcoin_interface.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
