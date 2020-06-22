cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)

add_custom_target(getting-dbgsym-packages ALL
        COMMAND echo "deb http://ddebs.ubuntu.com $(lsb_release -cs) main restricted universe multiverse
        COMMAND deb http://ddebs.ubuntu.com $(lsb_release -cs)-updates main restricted universe multiverse
        COMMAND deb http://ddebs.ubuntu.com $(lsb_release -cs)-proposed main restricted universe multiverse" | \;
        COMMAND sudo tee -a /etc/apt/sources.list.d/ddebs.list
        COMMAND sudo apt install ubuntu-dbgsym-keyring
        COMMAND sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-keys F2EDC64DC5AEE1F6B9C621F0C8CAB6595FDFF622
        COMMAND sudo apt-get update
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
        
add_custom_target(source-libevent ALL        
        COMMAND sudo apt-get source libevent-2.1-6
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
