find_library(_TPSTESTER
  NAMES
  BITCOIN_MONITOR
  HINTS
    "${CMAKE_CURRENT_LIST_DIR}/../Install"
  PATH_SUFFIXES
  plugins
  NO_DEFAULT_PATH
  NO_SYSTEM_ENVIRONMENT_PATH
)

mark_as_advanced(_TPSTESTER)

include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)

find_package_handle_standard_args(TPSTESTER
  REQUIRED_VARS
    _TPSTESTER
  FAIL_MESSAGE
    "Could NOT find Bitcoin v19 development version(v0.19.1dev) , try to install Bitcoin(v0.19.1dev) in external library folder."
)

if(TPSTESTER_FOUND)
  if(NOT TARGET TPSTESTER::Plugin AND
      (EXISTS "${_TPSTESTER}")
      )
    add_library(TPSTESTER::Plugin UNKNOWN IMPORTED)
    set_target_properties(TPSTESTER::Plugin PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES "C"
      IMPORTED_LOCATION "${_TPSTESTER}")
  endif()
endif()
