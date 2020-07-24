
if (NOT CMAKE_INSTALL_PREFIX OR
    CMAKE_INSTALL_PREFIX STREQUAL "/usr/local")
  set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/../../../Install")
endif()

file(GLOB_RECURSE BDB_4.8.30_SRCS ${CMAKE_CURRENT_SOURCE_DIR} *.c *.h)

add_custom_target(bdb_4.8.30 ALL
  DEPENDS ${CMAKE_INSTALL_PREFIX}/bdb_4.8.30/lib/libdb.so
  )

add_custom_command(OUTPUT ${CMAKE_INSTALL_PREFIX}/bdb_4.8.30/lib/libdb.so
  DEPENDS ${BDB_4.8.30_SRCS}
  COMMAND ./dist/configure --enable-cxx --with-pic --prefix=${CMAKE_INSTALL_PREFIX}/bdb_4.8.30
  COMMAND make install
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  )
  
