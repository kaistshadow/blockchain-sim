find_library(_RPC
  NAMES
  BITCOIN_MONITOR
  HINTS
    "${CMAKE_CURRENT_LIST_DIR}/../Install"
  PATH_SUFFIXES
  plugins
  NO_DEFAULT_PATH
  NO_SYSTEM_ENVIRONMENT_PATH
)

mark_as_advanced(_RPC)

include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)

find_package_handle_standard_args(MinerNodeRPC
  REQUIRED_VARS
    _RPC
  FAIL_MESSAGE
    "Could NOT find Bitcoin v19 development version(v0.19.1dev) , try to install Bitcoin(v0.19.1dev) in external library folder."
)

if(MinerNodeRPC_FOUND)
  if(NOT TARGET MinerNodeRPC::Plugin AND
      (EXISTS "${_RPC}")
      )
    add_library(MinerNodeRPC::Plugin UNKNOWN IMPORTED)
    set_target_properties(MinerNodeRPC::Plugin PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES "C"
      IMPORTED_LOCATION "${_RPC}")
  endif()
endif()
