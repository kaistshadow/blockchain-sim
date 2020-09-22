#.rst:
# FindGeth_1_9_1dev_gccgo
# -----------
#
# Find the Go-ethereum plugin of version 1.9.1 (gccgo)
#
# Imported Targets
# ^^^^^^^^^^^^^^^^
#
# This module defines the following :prop_tgt:`IMPORTED` targets:
#
# ``Geth_1_9_1dev_gccgo::Plugin``
#   The Plugin for Go-ethereum v1.9.1(gccgo), if found.
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module will set the following variables in your project:
#
# ``GETH_1_9_1DEV_GCCGO_FOUND``
#   System has the Go-ethereum plugin.
#
# Hints
# ^^^^^
#

find_library(_GETH_1_9_1DEV_GCCGO_PLUGIN
  NAMES
  geth_1.9.1dev_gccgo
  HINTS
    "${CMAKE_CURRENT_LIST_DIR}/../Install"
  PATH_SUFFIXES
  plugins
  NO_DEFAULT_PATH
  NO_SYSTEM_ENVIRONMENT_PATH
)

mark_as_advanced(_GETH_1_9_1DEV_GCCGO_PLUGIN)

include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)

find_package_handle_standard_args(Geth_1_9_1dev_gccgo
  REQUIRED_VARS
    _GETH_1_9_1DEV_GCCGO_PLUGIN
  FAIL_MESSAGE
    "Could NOT find go ethereum v1.9.1 gccgo version(v1.9.1dev), try to install go ethereum (v1.9.1dev_gccgo) in external library folder."
)

if(GETH_1_9_1DEV_GCCGO_FOUND)
  if(NOT TARGET Geth_1_9_1dev_gccgo::Plugin AND
      (EXISTS "${_GETH_1_9_1DEV_GCCGO_PLUGIN}")
      )
    add_library(Geth_1_9_1dev_gccgo::Plugin UNKNOWN IMPORTED)
    set_target_properties(Geth_1_9_1dev_gccgo::Plugin PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES "C"
      IMPORTED_LOCATION "${_GETH_1_9_1DEV_GCCGO_PLUGIN}")
  endif()
endif()
