cmake_minimum_required(VERSION 2.8)

add_custom_target(boost ALL
    COMMAND wget https://dl.bintray.com/boostorg/release/1.70.0/source/boost_1_70_0.tar.gz
    COMMAND tar -xzf boost_1_70_0.tar.gz
    COMMAND cd boost_1_70_0 && ./bootstrap.sh && ./b2 headers && ./b2 -j 5
    COMMAND export BOOST_ROOT=~/boost_1_70_0
    COMMAND rm -rf boost_1_70_0.tar.gz
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )


