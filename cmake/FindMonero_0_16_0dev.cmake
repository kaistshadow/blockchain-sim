#.rst:
# FindMonero_0_16_0dev
# -----------
#
# Find the Monero plugin of version 0.16.0 (development)
#
# Imported Targets
# ^^^^^^^^^^^^^^^^
#
# This module defines the following :prop_tgt:`IMPORTED` targets:
#
# ``Monero_0_16_0dev::Plugin``
#   The Plugin for Monero v0.16.0, if found.
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module will set the following variables in your project:
#
# ``MONERO_0_16_0DEV_FOUND``
#   System has the Monero plugin.
#
# Hints
# ^^^^^
#

find_library(_MONERO_0_16_0DEV_PLUGIN
  NAMES
  monerod
  HINTS
    "${CMAKE_CURRENT_LIST_DIR}/../Install"
  PATH_SUFFIXES
  plugins
  NO_DEFAULT_PATH
  NO_SYSTEM_ENVIRONMENT_PATH
)

mark_as_advanced(_MONERO_0_16_0DEV_PLUGIN)

include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)

find_package_handle_standard_args(Monero_0_16_0dev
  REQUIRED_VARS
    _MONERO_0_16_0DEV_PLUGIN
  FAIL_MESSAGE
    "Could NOT find monero v0.16.0 development version(v0.16.0dev), try to install monero (v0.16.0dev) in external library folder."
)

if(MONERO_0_16_0DEV_FOUND)
  if(NOT TARGET Monero_0_16_0dev::Plugin AND
      (EXISTS "${_MONERO_0_16_0DEV_PLUGIN}")
      )
    add_library(Monero_0_16_0dev::Plugin UNKNOWN IMPORTED)
    set_target_properties(Monero_0_16_0dev::Plugin PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES "C"
      IMPORTED_LOCATION "${_MONERO_0_16_0DEV_PLUGIN}")
  endif()
endif()
