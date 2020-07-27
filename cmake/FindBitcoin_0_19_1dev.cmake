#.rst:
# FindBitcoin_0_19_1dev
# -----------
#
# Find the Bitcoin plugin of version 0.19.1 (v0.19.1)
#
# Imported Targets
# ^^^^^^^^^^^^^^^^
#
# This module defines the following :prop_tgt:`IMPORTED` targets:
#
# ``Bitcoin_0_19_1dev::Plugin``
#   The Plugin for Bitcoin v0.19.1 with no wallet, if found.
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module will set the following variables in your project:
#
# ``BITCOIN_0_19_1DEV_FOUND``
#   System has the Bitcoin plugin.
#
# Hints
# ^^^^^
#

find_library(_BITCOIN_0_19_1DEV_PLUGIN
  NAMES
  BITCOIND_0.19.1DEV
  HINTS
    "${CMAKE_CURRENT_LIST_DIR}/../Install"
  PATH_SUFFIXES
  plugins
  NO_DEFAULT_PATH
  NO_SYSTEM_ENVIRONMENT_PATH
)

mark_as_advanced(_BITCOIN_0_19_1DEV_PLUGIN)

include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)

find_package_handle_standard_args(Bitcoin_0_19_1dev
  REQUIRED_VARS
    _BITCOIN_0_19_1DEV_PLUGIN
  FAIL_MESSAGE
    "Could NOT find Bitcoin v19 development version(v0.19.1dev) , try to install Bitcoin(v0.19.1dev) in external library folder."
)

if(BITCOIN_0_19_1DEV_FOUND)
  if(NOT TARGET Bitcoin_0_19_1dev::Plugin AND
      (EXISTS "${_BITCOIN_0_19_1DEV_PLUGIN}")
      )
    add_library(Bitcoin_0_19_1dev::Plugin UNKNOWN IMPORTED)
    set_target_properties(Bitcoin_0_19_1dev::Plugin PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES "C"
      IMPORTED_LOCATION "${_BITCOIN_0_19_1DEV_PLUGIN}")
  endif()
endif()
