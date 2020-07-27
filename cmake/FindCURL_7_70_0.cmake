#.rst:
# FindCURL_7_70_0
# -----------
#
# Find the CURL encryption library of version 7.70.0.
#
# Imported Targets
# ^^^^^^^^^^^^^^^^
#
# This module defines the following :prop_tgt:`IMPORTED` targets:
#
# ``CURL_7_70_0::CURL``
#   The CURL ``curl`` library, if found.
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module will set the following variables in your project:
#
# ``CURL_7_70_0_FOUND``
#   System has the CURL library.
# ``CURL_7_70_0_INCLUDE_DIR``
#   The CURL include directory.
# ``CURL_7_70_0_LIBRARIES``
#   CURL libraries.
#
# Hints
# ^^^^^
#

find_path(CURL_7_70_0_INCLUDE_DIR
  NAMES
    curl/curl.h
  HINTS
    "${CMAKE_CURRENT_LIST_DIR}/../Install/curl_7.70.0"
  PATH_SUFFIXES
    include
  NO_DEFAULT_PATH
  NO_SYSTEM_ENVIRONMENT_PATH
)

find_library(CURL_7_70_0_LIBRARY
  NAMES
  curl
  NAMES_PER_DIR
  HINTS
    "${CMAKE_CURRENT_LIST_DIR}/../Install/curl_7.70.0"
  PATH_SUFFIXES
  lib
  NO_DEFAULT_PATH
  NO_SYSTEM_ENVIRONMENT_PATH
)

find_library(OPENSSL_1_1_0H_SSL_LIBRARY
  NAMES
  ssl
  NAMES_PER_DIR
  HINTS
    "${CMAKE_CURRENT_LIST_DIR}/../Install/openssl_1.1.0h"
  PATH_SUFFIXES
  lib
  NO_DEFAULT_PATH
  NO_SYSTEM_ENVIRONMENT_PATH
)

find_library(OPENSSL_1_1_0H_CRYPTO_LIBRARY
  NAMES
  crypto
  NAMES_PER_DIR
  HINTS
    "${CMAKE_CURRENT_LIST_DIR}/../Install/openssl_1.1.0h"
  PATH_SUFFIXES
  lib
  NO_DEFAULT_PATH
  NO_SYSTEM_ENVIRONMENT_PATH
)

set(CURL_7_70_0_LIBRARIES ${CURL_7_70_0_LIBRARY} ${OPENSSL_1_1_0H_SSL_LIBRARY} ${OPENSSL_1_1_0H_CRYPTO_LIBRARY} )

include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)

find_package_handle_standard_args(CURL_7_70_0
  REQUIRED_VARS
    CURL_7_70_0_LIBRARY
    CURL_7_70_0_INCLUDE_DIR
  FAIL_MESSAGE
    "Could NOT find CURL(v7.70.0), try to install CURL(v7.70.0) in external library folder."
)

if(CURL_7_70_0_FOUND)
  if(NOT TARGET CURL_7_70_0::CURL)
    add_library(CURL_7_70_0::CURL UNKNOWN IMPORTED)
    set_target_properties(CURL_7_70_0::CURL PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${CURL_7_70_0_INCLUDE_DIR}"
      IMPORTED_LOCATION "${CURL_7_70_0_LIBRARY}")
  endif()
endif()
