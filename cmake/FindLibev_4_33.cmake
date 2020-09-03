#.rst:
# FindLibev_4_33
# -----------
#
# Find the libev library of version 4.33
#
# Imported Targets
# ^^^^^^^^^^^^^^^^
#
# This module defines the following :prop_tgt:`IMPORTED` targets:
#
# ``Libev_4_33::Libev``
#   The ``libev`` library, if found.
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module will set the following variables in your project:
#
# ``LIBEV_4_33_FOUND``
#   System has the libev library.
# ``LIBEV_4_33_INCLUDE_DIR``
#
# Hints
# ^^^^^
#

find_path(LIBEV_4_33_INCLUDE_DIR
        NAMES
        ev.h
        HINTS
        "${CMAKE_CURRENT_LIST_DIR}/../Install/libev_4.33"
        PATH_SUFFIXES
        include
        NO_DEFAULT_PATH
        NO_SYSTEM_ENVIRONMENT_PATH
        )

find_library(_LIBEV_4_33_LIBRARY
        NAMES
        ev
        NAMES_PER_DIR
        HINTS
        "${CMAKE_CURRENT_LIST_DIR}/../Install/libev_4.33"
        PATH_SUFFIXES
        lib
        NO_DEFAULT_PATH
        NO_SYSTEM_ENVIRONMENT_PATH
        )

include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)

find_package_handle_standard_args(LIBEV_4_33
        REQUIRED_VARS
        _LIBEV_4_33_LIBRARY
        LIBEV_4_33_INCLUDE_DIR
        FAIL_MESSAGE
        "Could NOT find libev(4.33), try to install libev(4.33) in external library folder."
        )

if(LIBEV_4_33_FOUND)
    if(NOT TARGET LIBEV_4_33::Libev)
        add_library(LIBEV_4_33::Libev UNKNOWN IMPORTED)
        set_target_properties(LIBEV_4_33::Libev PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${LIBEV_4_33_INCLUDE_DIR}"
                IMPORTED_LOCATION "${_LIBEV_4_33_LIBRARY}")
    endif()
endif()
