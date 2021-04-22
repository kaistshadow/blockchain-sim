## Example for catch2 unit test
add_library(Catch INTERFACE)
target_include_directories(Catch
        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/../lib)

add_executable(catch-example catch-example.cpp)
target_link_libraries(catch-example
        Catch
        BLEEP)

add_executable(catch-BLEEPlib catch-BLEEPlib.cpp)
target_link_libraries(catch-BLEEPlib
        Catch
        BLEEP)