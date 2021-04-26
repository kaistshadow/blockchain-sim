find_library(_LITECOIN_DEV
  NAMES
  LITECOIN.DEV
  HINTS
    "${CMAKE_CURRENT_LIST_DIR}/../Install"
  PATH_SUFFIXES
  plugins
  NO_DEFAULT_PATH
  NO_SYSTEM_ENVIRONMENT_PATH
)

mark_as_advanced(_LITECOIN_DEV)

include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)

find_package_handle_standard_args(LITECOIN__DEV_
  REQUIRED_VARS
    _LITECOIN_DEV
  FAIL_MESSAGE
    "Could NOT find Bitcoin v19 development version(v0.19.1dev) , try to install Bitcoin(v0.19.1dev) in external library folder."
)

if(LITECOIN_DEV_FOUND)
  if(NOT TARGET LITECOIN__DEV_::Plugin AND
      (EXISTS "${_LITECOIN_DEV}")
      )
    add_library(LITECOIN__DEV_::Plugin UNKNOWN IMPORTED)
    set_target_properties(LITECOIN__DEV_::Plugin PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES "C"
      IMPORTED_LOCATION "${_LITECOIN_DEV}")
  endif()
endif()
