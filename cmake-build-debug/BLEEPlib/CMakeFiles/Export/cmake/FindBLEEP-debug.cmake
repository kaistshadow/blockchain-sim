#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "BLEEP::BLEEP" for configuration "Debug"
set_property(TARGET BLEEP::BLEEP APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(BLEEP::BLEEP PROPERTIES
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libBLEEP.so.1"
  IMPORTED_SONAME_DEBUG "libBLEEP.so.1"
  )

list(APPEND _IMPORT_CHECK_TARGETS BLEEP::BLEEP )
list(APPEND _IMPORT_CHECK_FILES_FOR_BLEEP::BLEEP "${_IMPORT_PREFIX}/lib/libBLEEP.so.1" )

# Import target "BLEEP::NEWBLEEP" for configuration "Debug"
set_property(TARGET BLEEP::NEWBLEEP APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(BLEEP::NEWBLEEP PROPERTIES
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libNEWBLEEP.so.1"
  IMPORTED_SONAME_DEBUG "libNEWBLEEP.so.1"
  )

list(APPEND _IMPORT_CHECK_TARGETS BLEEP::NEWBLEEP )
list(APPEND _IMPORT_CHECK_FILES_FOR_BLEEP::NEWBLEEP "${_IMPORT_PREFIX}/lib/libNEWBLEEP.so.1" )

# Import target "BLEEP::SHADOW_INTERFACE" for configuration "Debug"
set_property(TARGET BLEEP::SHADOW_INTERFACE APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(BLEEP::SHADOW_INTERFACE PROPERTIES
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libSHADOW_INTERFACE.so"
  IMPORTED_SONAME_DEBUG "libSHADOW_INTERFACE.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS BLEEP::SHADOW_INTERFACE )
list(APPEND _IMPORT_CHECK_FILES_FOR_BLEEP::SHADOW_INTERFACE "${_IMPORT_PREFIX}/lib/libSHADOW_INTERFACE.so" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
