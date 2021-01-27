cmake_minimum_required( VERSION 3.5 )

####################################################
# Step 1.
# put the file you want to test source code in the first params 
####################################################
add_executable( 020-MultiFile
    test.cpp
)

####################################################
# Step 3.
# copy the result executable test file of Step 1 at present directory
####################################################
add_custom_target(020-MultiFile_copy ALL
    DEPENDS 020-MultiFile
    COMMAND ${CMAKE_COMMAND} -E copy 020-MultiFile ${CMAKE_CURRENT_SOURCE_DIR}
)

####################################################
# Step 2.
# This process is to link the test source code target with Catch2 librarry
####################################################
foreach( name 020-MultiFile)
    target_link_libraries( ${name} Catch2 Catch2WithMain )
    set_property(TARGET ${name} PROPERTY CXX_STANDARD 14)
    set_property(TARGET ${name} PROPERTY CXX_EXTENSIONS OFF)
endforeach()
