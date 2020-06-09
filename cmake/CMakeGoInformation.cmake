# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

# This should be included before the _INIT variables are
# used to initialize the cache.  Since the rule variables
# have if blocks on them, users can still define them here.
# But, it should still be after the platform file so changes can
# be made to those values.

include(CMakeLanguageInformation)

if(UNIX)
    set(CMAKE_Go_OUTPUT_EXTENSION .o)
else()
    set(CMAKE_Go_OUTPUT_EXTENSION .obj)
endif()


set(CMAKE_Go_ECHO_ALL "echo \"TARGET: <TARGET> TARGET_BASE: <TARGET_BASE> ")
set(CMAKE_Go_ECHO_ALL "${CMAKE_Go_ECHO_ALL} OBJECT: <OBJECT> OBJECTS: <OBJECTS> OBJECT_DIR: <OBJECT_DIR> SOURCE: <SOURCE> SOURCES: <SOURCES> ")
set(CMAKE_Go_ECHO_ALL "${CMAKE_Go_ECHO_ALL} LINK_LIBRARIES: <LINK_LIBRARIES> FLAGS: <FLAGS> LINK_FLAGS: <LINK_FLAGS> \"")


if(NOT CMAKE_Go_CREATE_SHARED_LIBRARY)
    set(CMAKE_Go_CREATE_SHARED_LIBRARY
            "echo \"CMAKE_Go_CREATE_SHARED_LIBRARY\""
            "${CMAKE_Go_ECHO_ALL}"
            )
endif()

if(NOT CMAKE_Go_CREATE_SHARED_MODULE)
    set(CMAKE_Go_CREATE_SHARED_MODULE
            "echo \"CMAKE_Go_CREATE_SHARED_MODULE\""
            "${CMAKE_Go_ECHO_ALL}"
            )
endif()

if(NOT CMAKE_Go_CREATE_STATIC_LIBRARY)
    set(CMAKE_Go_CREATE_STATIC_LIBRARY
            "echo \"CMAKE_Go_CREATE_STATIC_LIBRARY\""
            "${CMAKE_Go_ECHO_ALL}"
            )
endif()

if(NOT CMAKE_Go_COMPILE_OBJECT)
    set(CMAKE_Go_COMPILE_OBJECT
            "echo \"CMAKE_Go_COMPILE_OBJECT\""
            "${CMAKE_Go_ECHO_ALL}"
            "${CMAKE_Go_COMPILER} --emit obj <SOURCE> -o <OBJECT>")
endif()

if(NOT CMAKE_Go_LINK_EXECUTABLE)
    set(CMAKE_Go_LINK_EXECUTABLE
            "echo \"CMAKE_Go_LINK_EXECUTABLE\""
            "${CMAKE_Go_ECHO_ALL}"
            )
endif()

mark_as_advanced(
        CMAKE_Go_FLAGS
        CMAKE_Go_FLAGS_DEBUG
        CMAKE_Go_FLAGS_MINSIZEREL
        CMAKE_Go_FLAGS_RELEASE
        CMAKE_Go_FLAGS_RELWITHDEBINFO)

set(CMAKE_Go_INFORMATION_LOADED 1)

#if(CMAKE_USER_MAKE_RULES_OVERRIDE)
  # Save the full path of the file so try_compile can use it.
 # include(${CMAKE_USER_MAKE_RULES_OVERRIDE} RESULT_VARIABLE _override)
  #set(CMAKE_USER_MAKE_RULES_OVERRIDE "${_override}")
#endif()

#if(CMAKE_USER_MAKE_RULES_OVERRIDE_Go)
  # Save the full path of the file so try_compile can use it.
 #  include(${CMAKE_USER_MAKE_RULES_OVERRIDE_Go} RESULT_VARIABLE _override)
 #  set(CMAKE_USER_MAKE_RULES_OVERRIDE_Go "${_override}")
#endif()

# refer: /usr/share/cmake-3.7/Modules/CMakeCInformation.cmake

#if(NOT CMAKE_Go_COMPILE_OBJECT)
#	set(CMAKE_Go_COMPILE_OBJECT "<CMAKE_Go_COMPILER> <FLAGS> -o <OBJECT> -c <SOURCE> ")
#endif()

#if(NOT CMAKE_Go_LINK_EXECUTABLE)
#	set(CMAKE_Go_LINK_EXECUTABLE "<CMAKE_Go_COMPILER> -pthread <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
#endif()