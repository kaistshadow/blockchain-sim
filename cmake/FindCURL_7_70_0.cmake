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

# mark_as_advanced(CURL_7_70_0_LIBRARY OPENSSL_1_1_0H_CRYPTO_LIBRARY OPENSSL_1_1_0H_SSL_LIBRARY)

# compat defines
# set(OPENSSL_1_1_0H_SSL_LIBRARIES ${OPENSSL_1_1_0H_SSL_LIBRARY})
# set(OPENSSL_1_1_0H_CRYPTO_LIBRARIES ${OPENSSL_1_1_0H_CRYPTO_LIBRARY})
# set(CURL_7_70_0_LIBRARIES ${CURL_7_70_0_LIBRARY})



# # for versioning check
# function(from_hex HEX DEC)
#   string(TOUPPER "${HEX}" HEX)
#   set(_res 0)
#   string(LENGTH "${HEX}" _strlen)

#   while (_strlen GREATER 0)
#     math(EXPR _res "${_res} * 16")
#     string(SUBSTRING "${HEX}" 0 1 NIBBLE)
#     string(SUBSTRING "${HEX}" 1 -1 HEX)
#     if (NIBBLE STREQUAL "A")
#       math(EXPR _res "${_res} + 10")
#     elseif (NIBBLE STREQUAL "B")
#       math(EXPR _res "${_res} + 11")
#     elseif (NIBBLE STREQUAL "C")
#       math(EXPR _res "${_res} + 12")
#     elseif (NIBBLE STREQUAL "D")
#       math(EXPR _res "${_res} + 13")
#     elseif (NIBBLE STREQUAL "E")
#       math(EXPR _res "${_res} + 14")
#     elseif (NIBBLE STREQUAL "F")
#       math(EXPR _res "${_res} + 15")
#     else()
#       math(EXPR _res "${_res} + ${NIBBLE}")
#     endif()

#     string(LENGTH "${HEX}" _strlen)
#   endwhile()

#   set(${DEC} ${_res} PARENT_SCOPE)
# endfunction()

# if(CURL_7_70_0_INCLUDE_DIR AND EXISTS "${CURL_7_70_0_INCLUDE_DIR}/openssl/opensslv.h")
#   file(STRINGS "${CURL_7_70_0_INCLUDE_DIR}/openssl/opensslv.h" openssl_version_str
#        REGEX "^#[\t ]*define[\t ]+OPENSSL_VERSION_NUMBER[\t ]+0x([0-9a-fA-F])+.*")

#   if(openssl_version_str)
#     # The version number is encoded as 0xMNNFFPPS: major minor fix patch status
#     # The status gives if this is a developer or prerelease and is ignored here.
#     # Major, minor, and fix directly translate into the version numbers shown in
#     # the string. The patch field translates to the single character suffix that
#     # indicates the bug fix state, which 00 -> nothing, 01 -> a, 02 -> b and so
#     # on.

#     string(REGEX REPLACE "^.*OPENSSL_VERSION_NUMBER[\t ]+0x([0-9a-fA-F])([0-9a-fA-F][0-9a-fA-F])([0-9a-fA-F][0-9a-fA-F])([0-9a-fA-F][0-9a-fA-F])([0-9a-fA-F]).*$"
#            "\\1;\\2;\\3;\\4;\\5" OPENSSL_VERSION_LIST "${openssl_version_str}")
#     list(GET OPENSSL_VERSION_LIST 0 OPENSSL_VERSION_MAJOR)
#     list(GET OPENSSL_VERSION_LIST 1 OPENSSL_VERSION_MINOR)
#     from_hex("${OPENSSL_VERSION_MINOR}" OPENSSL_VERSION_MINOR)
#     list(GET OPENSSL_VERSION_LIST 2 OPENSSL_VERSION_FIX)
#     from_hex("${OPENSSL_VERSION_FIX}" OPENSSL_VERSION_FIX)
#     list(GET OPENSSL_VERSION_LIST 3 OPENSSL_VERSION_PATCH)

#     if (NOT OPENSSL_VERSION_PATCH STREQUAL "00")
#       from_hex("${OPENSSL_VERSION_PATCH}" _tmp)
#       # 96 is the ASCII code of 'a' minus 1
#       math(EXPR OPENSSL_VERSION_PATCH_ASCII "${_tmp} + 96")
#       unset(_tmp)
#       # Once anyone knows how OpenSSL would call the patch versions beyond 'z'
#       # this should be updated to handle that, too. This has not happened yet
#       # so it is simply ignored here for now.
#       string(ASCII "${OPENSSL_VERSION_PATCH_ASCII}" OPENSSL_VERSION_PATCH_STRING)
#     endif ()

#     set(OPENSSL_VERSION "${OPENSSL_VERSION_MAJOR}.${OPENSSL_VERSION_MINOR}.${OPENSSL_VERSION_FIX}${OPENSSL_VERSION_PATCH_STRING}")
#   endif ()
# endif ()

# if (NOT OPENSSL_VERSION STREQUAL "1.1.0h")
#   message(FATAL_ERROR "OPENSSL_VERSION=${OPENSSL_VERSION}")  
# endif ()

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
