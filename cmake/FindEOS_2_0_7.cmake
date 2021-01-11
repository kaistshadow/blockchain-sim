#.rst:
# FindEOS_2_0_7
# -----------
#
# Find the EOS plugin of version 2.0.7 (v2.0.7)
#
# Imported Targets
# ^^^^^^^^^^^^^^^^
#
# This module defines the following :prop_tgt:`IMPORTED` targets:
#
# ``EOS_2_0_7::Plugin``
#   The Plugin for EOS v2.0.7 with wallet, if found.
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module will set the following variables in your project:
#
# ``EOS_2_0_7_FOUND``
#   System has the EOS plugin.
#
# Hints
# ^^^^^
#


find_library(_EOS_2_0_7_LIBRARY
  NAMES
  NODEOS_2.0.7
  NAMES_PER_DIR
  HINTS
    "${CMAKE_CURRENT_LIST_DIR}/../Install/"
  PATH_SUFFIXES
  plugins
  NO_DEFAULT_PATH
  NO_SYSTEM_ENVIRONMENT_PATH
)

find_program(_EOS_2_0_7_EXECUTABLE
  NAMES
  nodeos
  NAMES_PER_DIR
  HINTS
    "${CMAKE_CURRENT_LIST_DIR}/../external/eosio/eos/build"
  PATH_SUFFIXES
  bin
  NO_DEFAULT_PATH
  NO_SYSTEM_ENVIRONMENT_PATH
)


mark_as_advanced(_EOS_2_0_7_LIBRARY _EOS_2_0_7_EXECUTABLE)


include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)

find_package_handle_standard_args(Eosio_2_0_7
  REQUIRED_VARS
    _EOS_2_0_7_LIBRARY
  FAIL_MESSAGE
    "Could NOT find EOS(v2.0.7), try to install EOS(v2.0.7) in external library folder."
)

if(EOSIO_2_0_7_FOUND)
  if(NOT TARGET Eosio_2_0_7::Bin AND
      (EXISTS "${_EOS_2_0_7_EXECUTABLE}")
      )
    add_library(Eosio_2_0_7::Bin UNKNOWN IMPORTED)
    if(EXISTS "${_EOS_2_0_7_EXECUTABLE}")
      set_target_properties(Eosio_2_0_7::Bin PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES "C"
        IMPORTED_LOCATION "${_EOS_2_0_7_EXECUTABLE}")
    endif()
  endif()
  if(NOT TARGET Eosio_2_0_7::Lib AND
      (EXISTS "${_EOS_2_0_7_LIBRARY}")
      )
    add_library(Eosio_2_0_7::Lib UNKNOWN IMPORTED)
    if(EXISTS "${_EOS_2_0_7_LIBRARY}")
      set_target_properties(Eosio_2_0_7::Lib PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES "C"
        IMPORTED_LOCATION "${_EOS_2_0_7_LIBRARY}")
    endif()
  endif()
endif()