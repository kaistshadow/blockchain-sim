find_library(ZCASH_DEV
  NAMES
  LITECOIN.DEV
  HINTS
    "${CMAKE_CURRENT_LIST_DIR}/../Install"
  PATH_SUFFIXES
  plugins
  NO_DEFAULT_PATH
  NO_SYSTEM_ENVIRONMENT_PATH
)

mark_as_advanced(ZCASH_DEV)

include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)

find_package_handle_standard_args(_ZCASH_DEV_
  REQUIRED_VARS
    ZCASH_DEV
  FAIL_MESSAGE
    "Could NOT find Bitcoin v19 development version(v0.19.1dev) , try to install Bitcoin(v0.19.1dev) in external library folder."
)

if(NOT TARGET _ZCASH_DEV_::Plugin AND
    (EXISTS "${ZCASH_DEV}")
    )
add_library(_ZCASH_DEV_::Plugin UNKNOWN IMPORTED)
set_target_properties(_ZCASH_DEV_::Plugin PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES "C"
    IMPORTED_LOCATION "${ZCASH_DEV}")
endif()
