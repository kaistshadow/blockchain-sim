cmake_minimum_required(VERSION 2.8)

add_custom_target(shadow-make-monero ALL
  COMMAND make -j6 debug
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/dev0.16.0)

