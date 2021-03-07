#.rst:
# FindShadow
# -----------
#
# Find the Shadow binary
#
# Imported Targets
# ^^^^^^^^^^^^^^^^
#
# This module defines the following :prop_tgt:`IMPORTED` targets:
#
# ``shadow``
#   The Binary for shadow, if found.
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module will set the following variables in your project:
#
# ``SHADOW_FOUND``
#   System has the Bitcoin plugin for v0.19.1 dev.

find_program(_SHADOW
        NAMES
        shadow
        HINTS
        "${CMAKE_CURRENT_LIST_DIR}/../Install"
        PATH_SUFFIXES
        bin
        NO_DEFAULT_PATH
        NO_SYSTEM_ENVIRONMENT_PATH
        )


include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)

find_package_handle_standard_args(shadow
        REQUIRED_VARS
        _SHADOW
        FAIL_MESSAGE
        "Could NOT find shadow binary , try to install shadow first."
        )

if(shadow_FOUND)
    if(NOT TARGET shadow)
        add_executable(shadow IMPORTED)
        set_target_properties(shadow PROPERTIES
                IMPORTED_LINK_INTERFACE_LANGUAGES "C"
                IMPORTED_LOCATION "${_SHADOW}")
    endif()
endif()
