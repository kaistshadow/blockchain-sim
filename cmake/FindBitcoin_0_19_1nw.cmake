#.rst:
# FindBitcoin_0_19_1nw
# -----------
#
# Find the Bitcoin plugin of version 0.19.1nw (v0.19.1 with no wallet)
#
# Imported Targets
# ^^^^^^^^^^^^^^^^
#
# This module defines the following :prop_tgt:`IMPORTED` targets:
#
# ``Bitcoin_0_19_1nw::Plugin``
#   The Plugin for Bitcoin v0.19.1 with no wallet, if found.
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module will set the following variables in your project:
#
# ``BITCOIN_0_19_1NW_FOUND``
#   System has the Bitcoin plugin.
# ``BITCOIN_0_19_1NW_PLUGIN``
#   The Bitcoin Plugin.
#
# Hints
# ^^^^^
#

find_library(BITCOIN_0_19_1NW_PLUGIN
  NAMES
  BITCOIND_0.19.1nw
  HINTS
    "${CMAKE_CURRENT_LIST_DIR}/../Install"
  PATH_SUFFIXES
  plugins
  NO_DEFAULT_PATH
  NO_SYSTEM_ENVIRONMENT_PATH
)

mark_as_advanced(BITCOIN_0_19_1NW_PLUGIN)

include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)

find_package_handle_standard_args(Bitcoin_0_19_1nw
  REQUIRED_VARS
    BITCOIN_0_19_1NW_PLUGIN
  FAIL_MESSAGE
    "Could NOT find Bitcoin(v0.19.1 without wallet), try to install Bitcoin(v0.19.1 without wallet) in external library folder."
)

if(BITCOIN_0_19_1NW_FOUND)
  if(NOT TARGET Bitcoin_0_19_1nw::Plugin AND
      (EXISTS "${BITCOIN_0_19_1NW_PLUGIN}")
      )
    add_library(Bitcoin_0_19_1nw::Plugin UNKNOWN IMPORTED)
    set_target_properties(Bitcoin_0_19_1nw::Plugin PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES "C"
      IMPORTED_LOCATION "${BITCOIN_0_19_1NW_PLUGIN}")
  endif()
endif()
