if (NOT CMAKE_INSTALL_PREFIX OR
        CMAKE_INSTALL_PREFIX STREQUAL "/usr/local")
    set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/../../../Install")
endif()
file(GLOB_RECURSE BITCOIN_0_10_0DEV_SRCS ${CMAKE_CURRENT_SOURCE_DIR} *.c *.cpp)
add_custom_target(bitcoin_0.10.0dev ALL
        DEPENDS ${CMAKE_INSTALL_PREFIX}/plugins/libBITCOIND_0.10.0DEV.so
        )
add_custom_command(OUTPUT ${CMAKE_INSTALL_PREFIX}/plugins/libBITCOIND_0.10.0DEV.so
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/bitcoind
        DEPENDS openssl_1.0.1k
        COMMAND g++ -g3 -O0 -DDEBUG -Wall -Wextra -Wformat -Wformat-security -Wno-unused-parameter -Wstack-protector -fstack-protector-all -fPIC -Wl,--exclude-libs -Wl,ALL -pthread -Wl,-z -Wl,relro -Wl,-z -Wl,now -fPIC -shared -rdynamic -Wl,-rpath=${CMAKE_INSTALL_PREFIX}/bdb_4.8.30/lib -Wl,-rpath=${CMAKE_INSTALL_PREFIX}/lib -o libBITCOIND.so bitcoind-bitcoind.o -L${CMAKE_INSTALL_PREFIX}/openssl_1.0.1k/lib -L${CMAKE_INSTALL_PREFIX}/bdb_4.8.30/lib -L${CMAKE_INSTALL_PREFIX}/lib -L$<TARGET_FILE_DIR:SHADOW_INTERFACE> -L$<TARGET_FILE_DIR:SHADOW_BITCOIN_INTERFACE> libbitcoin_server.a libbitcoin_common.a univalue/libbitcoin_univalue.a libbitcoin_util.a crypto/libbitcoin_crypto.a ./leveldb/libleveldb.a ./leveldb/libmemenv.a secp256k1/.libs/libsecp256k1.a libbitcoin_wallet.a -L/usr/lib/x86_64-linux-gnu -lboost_system -lboost_filesystem -lboost_program_options -lboost_thread -lboost_chrono -lssl -lcrypto -lminiupnpc -lanl -pthread -lSHADOW_INTERFACE -lSHADOW_BITCOIN_INTERFACE -D__LIBTOOL_IS_A_FOOL__
        COMMAND ${CMAKE_COMMAND} -E copy libBITCOIND.so ${CMAKE_INSTALL_PREFIX}/plugins/libBITCOIND_0.10.0DEV.so
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src
        )
add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/bitcoind
        DEPENDS bdb_4.8.30
        DEPENDS openssl_1.0.1k
        DEPENDS ${BITCOIN_0_10_0DEV_SRCS}
        DEPENDS SHADOW_INTERFACE
        DEPENDS SHADOW_BITCOIN_INTERFACE
        COMMAND ./autogen.sh
        COMMAND ./configure CPPFLAGS='-I${CMAKE_INSTALL_PREFIX}/bdb_4.8.30/include -I${CMAKE_INSTALL_PREFIX}/openssl_1.0.1k/include -I${CMAKE_ROOT_SOURCE_DIR}/interfaces/shadow_interface -I${CMAKE_ROOT_SOURCE_DIR}/interfaces/shadow_bitcoin_interface -I${CMAKE_INSTALL_PREFIX}/include' LDFLAGS='-L${CMAKE_INSTALL_PREFIX}/bdb_4.8.30/lib -L${CMAKE_INSTALL_PREFIX}/openssl_1.0.1k/lib -L$<TARGET_FILE_DIR:SHADOW_INTERFACE> -L$<TARGET_FILE_DIR:SHADOW_BITCOIN_INTERFACE> -L${CMAKE_INSTALL_PREFIX}/lib' LIBS='-lSHADOW_INTERFACE -lSHADOW_BITCOIN_INTERFACE' --with-pic --disable-bench --disable-tests --enable-debug --disable-threadlocal --disable-zmq
        COMMAND make -j4
        COMMAND touch ${CMAKE_CURRENT_SOURCE_DIR}/src/bitcoind
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
# Define targets that can be used in same build-tree
add_library(Bitcoin_0_10_0dev::Plugin SHARED IMPORTED GLOBAL)
add_dependencies(Bitcoin_0_10_0dev::Plugin bitcoin_0.10.0dev)
set_target_properties(Bitcoin_0_10_0dev::Plugin PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES "C"
        IMPORTED_LOCATION ${CMAKE_INSTALL_PREFIX}/plugins/libBITCOIND_0.10.0DEV.so
        INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/src)
set_property(TARGET Bitcoin_0_10_0dev::Plugin PROPERTY IMPORTED_NO_SONAME 1)