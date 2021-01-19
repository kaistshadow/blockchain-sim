add_subdirectory(15.6.0)

# Define targets that can be used in same build-tree
add_library(Nodejs::Libnode SHARED IMPORTED GLOBAL)
add_dependencies(Nodejs::Libnode nodejs)
set_target_properties(Nodejs::Libnode PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES "C"
        IMPORTED_LOCATION ${CMAKE_INSTALL_PREFIX}/nodejs/lib/libnode.so)


add_library(nodejs_plugin SHARED run-nodejs.cpp)
target_link_libraries(nodejs_plugin Nodejs::Libnode)

add_library(Nodejs::Plugin ALIAS nodejs_plugin)