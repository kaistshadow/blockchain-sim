
if (NOT CMAKE_INSTALL_PREFIX OR
    CMAKE_INSTALL_PREFIX STREQUAL "/usr/local")
  set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/../../../Install")
endif()


file(GLOB_RECURSE BITCOIN_0_19_1nw_SRCS ${CMAKE_CURRENT_SOURCE_DIR} *.c *.h *.cpp)

add_custom_target(bitcoin_0.19.1nw ALL
  DEPENDS ${CMAKE_INSTALL_PREFIX}/plugins/libBITCOIND_0.19.1nw.so
  )

add_custom_command(OUTPUT ${CMAKE_INSTALL_PREFIX}/plugins/libBITCOIND_0.19.1nw.so
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/bitcoind
  DEPENDS openssl_1.1.0h
  COMMAND g++ -std=c++11 -O0 -g3 -ftrapv -fstack-reuse=none -Wstack-protector -fstack-protector-all -Wall -Wextra -Wformat -Wvla -Wswitch -Wformat-security -Wredundant-decls -Wno-unused-parameter -Wno-implicit-fallthrough -fPIC -pthread -Wl,-z -Wl,relro -Wl,-z -Wl,now -fPIC -shared -rdynamic -Wl,-rpath=${CMAKE_INSTALL_PREFIX}/openssl_1.1.0h/lib -o libBITCOIND.so bitcoind-bitcoind.o -L${CMAKE_INSTALL_PREFIX}/openssl_1.1.0h/lib libbitcoin_server.a libbitcoin_server.a libbitcoin_common.a univalue/.libs/libunivalue.a libbitcoin_util.a libbitcoin_consensus.a crypto/libbitcoin_crypto_base.a crypto/libbitcoin_crypto_sse41.a crypto/libbitcoin_crypto_avx2.a crypto/libbitcoin_crypto_shani.a leveldb/libleveldb.a leveldb/libleveldb_sse42.a leveldb/libmemenv.a secp256k1/.libs/libsecp256k1.a -L/usr/lib/x86_64-linux-gnu -lboost_system -lboost_filesystem -lboost_thread -lpthread -lboost_chrono -ldb_cxx -lcrypto -levent_pthreads -levent -levent -pthread
  COMMAND ${CMAKE_COMMAND} -E copy libBITCOIND.so ${CMAKE_INSTALL_PREFIX}/plugins/libBITCOIND_0.19.1nw.so
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src
  )

add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/bitcoind
  DEPENDS ${BITCOIN_0_19_1nw_SRCS}
  COMMAND ./autogen.sh
  COMMAND ./configure --with-pic --disable-bench --disable-tests --enable-debug --disable-wallet --disable-zmq
  COMMAND make -j4
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
  

# Define targets that can be used in same build-tree
add_library(Bitcoin_0_19_1nw::Plugin SHARED IMPORTED GLOBAL)
add_dependencies(Bitcoin_0_19_1nw::Plugin bitcoin_0.19.1nw)
set_target_properties(Bitcoin_0_19_1nw::Plugin PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES "C"
  IMPORTED_LOCATION ${CMAKE_INSTALL_PREFIX}/plugins/libBITCOIND_0.19.1nw.so)
