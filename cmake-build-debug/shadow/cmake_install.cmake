# Install script for directory: /home/victor/blockchain-sim/shadow

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/victor/blockchain-sim/shadow/../Install")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES "/home/victor/blockchain-sim/cmake-build-debug/shadow/shd-config.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/cmake/Modules" TYPE FILE FILES
    "/home/victor/blockchain-sim/shadow/cmake/FindDL.cmake"
    "/home/victor/blockchain-sim/shadow/cmake/FindGLIB.cmake"
    "/home/victor/blockchain-sim/shadow/cmake/FindIGRAPH.cmake"
    "/home/victor/blockchain-sim/shadow/cmake/FindM.cmake"
    "/home/victor/blockchain-sim/shadow/cmake/FindRPTH.cmake"
    "/home/victor/blockchain-sim/shadow/cmake/FindRT.cmake"
    "/home/victor/blockchain-sim/shadow/cmake/ShadowTools.cmake"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/victor/blockchain-sim/cmake-build-debug/shadow/resource/cmake_install.cmake")
  include("/home/victor/blockchain-sim/cmake-build-debug/shadow/src/cmake_install.cmake")
  include("/home/victor/blockchain-sim/cmake-build-debug/shadow/cpack/cmake_install.cmake")

endif()

