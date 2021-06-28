add_library(SHADOW_MEMSHARE_INTERFACE SHARED shadow_memshare_interface.cpp)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3")
add_dependencies(SHADOW_MEMSHARE_INTERFACE SHADOW_INTERFACE)
target_link_libraries(SHADOW_MEMSHARE_INTERFACE SHADOW_INTERFACE)
install(TARGETS SHADOW_MEMSHARE_INTERFACE EXPORT FindBLEEP DESTINATION lib) # Why export here? : https://stackoverflow.com/a/5379238

target_include_directories(SHADOW_MEMSHARE_INTERFACE
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

include_directories(${CMAKE_ROOT_SOURCE_DIR}/interfaces/shadow_interface)

install(FILES shadow_memshare_interface.h
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
