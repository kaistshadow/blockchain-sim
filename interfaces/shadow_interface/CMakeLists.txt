add_library(SHADOW_INTERFACE SHARED shadow_interface.c)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3")


install(TARGETS SHADOW_INTERFACE EXPORT FindBLEEP DESTINATION lib) # Why export here? : https://stackoverflow.com/a/5379238

target_include_directories(SHADOW_INTERFACE
  PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

install(FILES shadow_interface.h 
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})