#.rst:
# FindMonitorNode
# -----------
#
# Find the Bitcoin plugin of version 0.19.1 (v0.19.1) development version
#
# Imported Targets
# ^^^^^^^^^^^^^^^^
#
# This module defines the following :prop_tgt:`IMPORTED` targets:
#
# ``MonitorNode::Plugin``
#   The Plugin for Bitcoin v0.19.1 development version, if found.
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module will set the following variables in your project:
#
# ``MonitorNode_FOUND``
#   System has the Bitcoin plugin for v0.19.1 dev.
#
# Hints
# ^^^^^
#

find_library(_MONITORNODE
  NAMES
  BITCOIN_MONITOR
  HINTS
    "${CMAKE_CURRENT_LIST_DIR}/../Install"
  PATH_SUFFIXES
  plugins
  NO_DEFAULT_PATH
  NO_SYSTEM_ENVIRONMENT_PATH
)

mark_as_advanced(_MONITORNODE)

include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)

find_package_handle_standard_args(MonitorNode
  REQUIRED_VARS
    _MONITORNODE
  FAIL_MESSAGE
    "Could NOT find Bitcoin v19 development version(v0.19.1dev) , try to install Bitcoin(v0.19.1dev) in external library folder."
)

if(MonitorNode_FOUND)
  if(NOT TARGET MonitorNode::Plugin AND
      (EXISTS "${_MONITORNODE}")
      )
    add_library(MonitorNode::Plugin UNKNOWN IMPORTED)
    set_target_properties(MonitorNode::Plugin PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES "C"
      IMPORTED_LOCATION "${_MONITORNODE}")
  endif()
endif()
