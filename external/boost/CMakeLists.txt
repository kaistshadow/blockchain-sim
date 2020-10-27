cmake_minimum_required(VERSION 2.8)

if (NOT CMAKE_INSTALL_PREFIX OR
        CMAKE_INSTALL_PREFIX STREQUAL "/usr/local")
    set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/../../Install")
endif()

add_custom_target(boost_0_17_0 ALL
        DEPENDS ${CMAKE_INSTALL_PREFIX}/boost
        )

add_custom_target(boost_0_17_0_install ALL
    COMMAND wget https://dl.bintray.com/boostorg/release/1.70.0/source/boost_1_70_0.tar.gz
    COMMAND tar -xzf boost_1_70_0.tar.gz
    COMMAND cd boost_1_70_0 && ./bootstrap.sh && ./b2 headers && ./b2 -j 5
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )

# add_custom_command(OUTPUT_BOOST ${CMAKE_CURRENT_SOURCE_DIR}/boost/boost_1_70_0/libboost_regex.so
#         COMMAND wget https://dl.bintray.com/boostorg/release/1.70.0/source/boost_1_70_0.tar.gz
#         COMMAND tar -xzf boost_1_70_0.tar.gz
#         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#        )


# add_custom_command(OUTPUT2 ${CMAKE_CURRENT_SOURCE_DIR}/boost_1_70_0/stage/lib/libboost_regex.so
#         DEPENDS ${CMAKE_INSTALL_PREFIX}/boost/libboost_regex.so
#         COMMAND cd boost_1_70_0 && ./bootstrap.sh && ./b2 headers && ./b2 -j 5
#         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/boost_1_70_0
#         )

