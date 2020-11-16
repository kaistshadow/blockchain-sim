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

find_library(_EOS_2_0_7_PLUGIN
  NAMES
  EOS_2.0.7
  HINTS
    "${CMAKE_CURRENT_LIST_DIR}/../Install"
  PATH_SUFFIXES
  plugins
  NO_DEFAULT_PATH
  NO_SYSTEM_ENVIRONMENT_PATH
)

mark_as_advanced(_EOS_2_0_7_PLUGIN)

include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)

find_package_handle_standard_args(EOS_2_0_7
  REQUIRED_VARS
    _EOS_2_0_7_PLUGIN
  FAIL_MESSAGE
    "Could NOT find EOS(v2.0.7), try to install EOS(v2.0.7) in external library folder."
)

if(EOS_2_0_7_FOUND)
  if(NOT TARGET EOS_2_0_7::Plugin AND
      (EXISTS "${_EOS_2_0_7_PLUGIN}")
      )
    add_library(EOS_2_0_7::Plugin UNKNOWN IMPORTED)
    set_target_properties(EOS_2_0_7::Plugin PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES "C"
      IMPORTED_LOCATION "${_EOS_2_0_7_PLUGIN}")
  endif()
endif()
