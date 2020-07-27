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
# ``OpenSSL_1_1_0h::Crypto``
#   The OpenSSL ``crypto`` library, if found.
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module will set the following variables in your project:
#
# ``OPENSSL_1_1_0H_FOUND``
#   System has the OpenSSL library.
#
# Hints
# ^^^^^
#

find_path(_OPENSSL_1_1_0H_INCLUDE_DIR
  NAMES
    openssl/ssl.h
  HINTS
    "${CMAKE_CURRENT_LIST_DIR}/../Install/openssl_1.1.0h"
  PATH_SUFFIXES
    include
  NO_DEFAULT_PATH
  NO_SYSTEM_ENVIRONMENT_PATH
)


find_library(_OPENSSL_1_1_0H_SSL_LIBRARY
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

find_library(_OPENSSL_1_1_0H_CRYPTO_LIBRARY
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

mark_as_advanced(_OPENSSL_1_1_0H_CRYPTO_LIBRARY _OPENSSL_1_1_0H_SSL_LIBRARY)

# set(_OPENSSL_1_1_0H_LIBRARIES ${_OPENSSL_1_1_0H_SSL_LIBRARY} ${_OPENSSL_1_1_0H_CRYPTO_LIBRARY} )

include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)

find_package_handle_standard_args(OpenSSL_1_1_0h
  REQUIRED_VARS
    #OPENSSL_SSL_LIBRARY # FIXME: require based on a component request?
    _OPENSSL_1_1_0H_CRYPTO_LIBRARY
    _OPENSSL_1_1_0H_INCLUDE_DIR
  FAIL_MESSAGE
    "Could NOT find OpenSSL(v1.1.0h), try to install OpenSSL(v1.1.0h) in external library folder."
)

if(OPENSSL_1_1_0H_FOUND)
  if(NOT TARGET OpenSSL_1_1_0h::Crypto AND
      (EXISTS "${_OPENSSL_1_1_0H_CRYPTO_LIBRARY}")
      )
    add_library(OpenSSL_1_1_0h::Crypto UNKNOWN IMPORTED)
    set_target_properties(OpenSSL_1_1_0h::Crypto PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${_OPENSSL_1_1_0H_INCLUDE_DIR}")
    if(EXISTS "${_OPENSSL_1_1_0H_CRYPTO_LIBRARY}")
      set_target_properties(OpenSSL_1_1_0h::Crypto PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES "C"
        IMPORTED_LOCATION "${_OPENSSL_1_1_0H_CRYPTO_LIBRARY}")
    endif()
  endif()
  if(NOT TARGET OpenSSL_1_1_0h::SSL AND
      (EXISTS "${_OPENSSL_1_1_0H_SSL_LIBRARY}")
      )
    add_library(OpenSSL_1_1_0h::SSL UNKNOWN IMPORTED)
    set_target_properties(OpenSSL_1_1_0h::SSL PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${_OPENSSL_1_1_0H_INCLUDE_DIR}")
    if(EXISTS "${_OPENSSL_1_1_0H_SSL_LIBRARY}")
      set_target_properties(OpenSSL_1_1_0h::SSL PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES "C"
        IMPORTED_LOCATION "${_OPENSSL_1_1_0H_SSL_LIBRARY}")
    endif()
    if(TARGET OpenSSL_1_1_0h::Crypto)
      set_target_properties(OpenSSL_1_1_0h::SSL PROPERTIES
        INTERFACE_LINK_LIBRARIES OpenSSL_1_1_0h::Crypto)
    endif()
  endif()
endif()
