#.rst:
# FindOpenSSL_1_1_0h
# -----------
#
# Find the OpenSSL encryption library of version 1.1.0h.
#
# Imported Targets
# ^^^^^^^^^^^^^^^^
#
# This module defines the following :prop_tgt:`IMPORTED` targets:
#
# ``OpenSSL_1_1_0h::SSL``
#   The OpenSSL ``ssl`` library, if found.
# ``OpenSSL_1_10h::Crypto``
#   The OpenSSL ``crypto`` library, if found.
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module will set the following variables in your project:
#
# ``OPENSSL_1_1_0h_FOUND``
#   System has the OpenSSL library.
# ``OPENSSL_1_1_0h_INCLUDE_DIR``
#   The OpenSSL include directory.
# ``OPENSSL_1_1_0h_CRYPTO_LIBRARY``
#   The OpenSSL crypto library.
# ``OPENSSL_1_1_0h_SSL_LIBRARY``
#   The OpenSSL SSL library.
# ``OPENSSL_1_1_0h_LIBRARIES``
#   All OpenSSL libraries.
#
# Hints
# ^^^^^
#

find_path(OPENSSL_1_1_0h_INCLUDE_DIR
  NAMES
    openssl/ssl.h
  HINTS
    "${CMAKE_CURRENT_LIST_DIR}/../Install/openssl_1.1.0h"
  PATH_SUFFIXES
    include
  NO_DEFAULT_PATH
  NO_SYSTEM_ENVIRONMENT_PATH
)


find_library(OPENSSL_1_1_0h_SSL_LIBRARY
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

find_library(OPENSSL_1_1_0h_CRYPTO_LIBRARY
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

mark_as_advanced(OPENSSL_1_1_0h_CRYPTO_LIBRARY OPENSSL_1_1_0h_SSL_LIBRARY)

# compat defines
set(OPENSSL_1_1_0h_SSL_LIBRARIES ${OPENSSL_1_1_0h_SSL_LIBRARY})
set(OPENSSL_1_1_0h_CRYPTO_LIBRARIES ${OPENSSL_1_1_0h_CRYPTO_LIBRARY})


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

# if(OPENSSL_1_1_0h_INCLUDE_DIR AND EXISTS "${OPENSSL_1_1_0h_INCLUDE_DIR}/openssl/opensslv.h")
#   file(STRINGS "${OPENSSL_1_1_0h_INCLUDE_DIR}/openssl/opensslv.h" openssl_version_str
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


set(OPENSSL_1_1_0h_LIBRARIES ${OPENSSL_1_1_0h_SSL_LIBRARY} ${OPENSSL_1_1_0h_CRYPTO_LIBRARY} )

include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)

find_package_handle_standard_args(OPENSSL_1_1_0h
  REQUIRED_VARS
    #OPENSSL_SSL_LIBRARY # FIXME: require based on a component request?
    OPENSSL_1_1_0h_CRYPTO_LIBRARY
    OPENSSL_1_1_0h_INCLUDE_DIR
  FAIL_MESSAGE
    "Could NOT find OpenSSL, try to install OpenSSL in external library folder."
)

if(OPENSSL_1_1_0h_FOUND)
  if(NOT TARGET OpenSSL_1_1_0h::Crypto AND
      (EXISTS "${OPENSSL_1_1_0h_CRYPTO_LIBRARY}")
      )
    add_library(OpenSSL_1_1_0h::Crypto UNKNOWN IMPORTED)
    set_target_properties(OpenSSL_1_1_0h::Crypto PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${OPENSSL_1_1_0h_INCLUDE_DIR}")
    if(EXISTS "${OPENSSL_1_1_0h_CRYPTO_LIBRARY}")
      set_target_properties(OpenSSL_1_1_0h::Crypto PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES "C"
        IMPORTED_LOCATION "${OPENSSL_1_1_0h_CRYPTO_LIBRARY}")
    endif()
  endif()
  if(NOT TARGET OpenSSL_1_1_0h::SSL AND
      (EXISTS "${OPENSSL_1_1_0h_SSL_LIBRARY}")
      )
    add_library(OpenSSL_1_1_0h::SSL UNKNOWN IMPORTED)
    set_target_properties(OpenSSL_1_1_0h::SSL PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${OPENSSL_1_1_0h_INCLUDE_DIR}")
    if(EXISTS "${OPENSSL_1_1_0h_SSL_LIBRARY}")
      set_target_properties(OpenSSL_1_1_0h::SSL PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES "C"
        IMPORTED_LOCATION "${OPENSSL_1_1_0h_SSL_LIBRARY}")
    endif()
    if(TARGET OpenSSL_1_1_0h::Crypto)
      set_target_properties(OpenSSL_1_1_0h::SSL PROPERTIES
        INTERFACE_LINK_LIBRARIES OpenSSL_1_1_0h::Crypto)
    endif()
  endif()
endif()
