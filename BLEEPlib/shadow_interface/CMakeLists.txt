add_library(SHADOW_INTERFACE SHARED shadow_interface.c)
install(TARGETS SHADOW_INTERFACE EXPORT FindBLEEP DESTINATION lib) # Why export here? : https://stackoverflow.com/a/5379238

message(STATUS "CMAKE_CURRENT_SOURCE_DIR:" ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(SHADOW_INTERFACE
  PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

install(FILES shadow_interface.h 
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})