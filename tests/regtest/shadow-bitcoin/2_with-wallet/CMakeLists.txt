## required for independent build
cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)

include(ExternalProject)

# SOURCE_DIR = <prefix>/src/<name>
# BINARY_DIR = <prefix>/src/<name>-build
# INSTALL_DIR = <prefix>
ExternalProject_Add(openssl-2
        URL https://www.openssl.org/source/openssl-1.1.0h.tar.gz
        CONFIGURE_COMMAND <SOURCE_DIR>/config --prefix=${CMAKE_CURRENT_BINARY_DIR} shared threads enable-ec_nistp_64_gcc_128 -fPIC
        BUILD_COMMAND make depend
        COMMAND make
        COMMAND make install_sw
        COMMAND make clean
        BUILD_IN_SOURCE TRUE
        INSTALL_COMMAND ""
        )

ExternalProject_add(bdb-4.8
        URL http://download.oracle.com/berkeley-db/db-4.8.30.NC.tar.gz
        CONFIGURE_COMMAND <SOURCE_DIR>/dist/configure --enable-cxx --with-pic --prefix=${CMAKE_CURRENT_BINARY_DIR}
        BUILD_COMMAND make install
        INSTALL_COMMAND ""
        )

ExternalProject_Add(bitcoin-with-wallet
        GIT_REPOSITORY https://github.com/bitcoin/bitcoin.git
        GIT_TAG v0.19.1
        PATCH_COMMAND <SOURCE_DIR>/autogen.sh
        COMMAND <SOURCE_DIR>/configure CPPFLAGS=-I${CMAKE_CURRENT_BINARY_DIR}/include LDFLAGS=-L${CMAKE_CURRENT_BINARY_DIR}/lib --with-pic --prefix=<INSTALL_DIR> --disable-bench --disable-tests --enable-debug
        CONFIGURE_COMMAND ""
        BUILD_COMMAND make -j8
        BUILD_IN_SOURCE TRUE
        INSTALL_COMMAND cp src/libBITCOIND.so ${CMAKE_CURRENT_SOURCE_DIR}
        )

ExternalProject_Add_Step(bitcoin-with-wallet PATCH_FOR_PIC
        COMMAND sed -i "s/PIE_FLAGS/PIC_FLAGS/g" Makefile.am
        COMMAND sed -i "s/PIE_FLAGS/PIC_FLAGS/g" Makefile.leveldb.include
        COMMENT "Patch Makefiles in order to use fPIC instead of fPIE."
        DEPENDEES patch
        DEPENDERS build
        WORKING_DIRECTORY <SOURCE_DIR>/src
        )

ExternalProject_Add_Step(bitcoin-with-wallet PATCH_FOR_SOURCE
#        COMMAND git apply ${CMAKE_CURRENT_SOURCE_DIR}/../DisableSanityCheck.patch || echo "skip DisableSanityCheck.patch"
        COMMAND git apply ${CMAKE_CURRENT_SOURCE_DIR}/../InfiniteLoop.patch || echo "skip InfiniteLoop.patch"
        COMMENT "Patch bitcoin src to resolve infinite execution."
        DEPENDEES patch
        DEPENDERS build
        WORKING_DIRECTORY <SOURCE_DIR>
        )

ExternalProject_Add_Step(bitcoin-with-wallet BUILD_LIBBITCOIND
        #COMMAND g++ -std=c++11 -O0 -g3 -ftrapv -fstack-reuse=none -Wstack-protector -fstack-protector-all -Wall -Wextra -Wformat -Wvla -Wswitch -Wformat-security -Wredundant-decls -Wno-unused-parameter -Wno-implicit-fallthrough -fPIC -pthread -Wl,-z -Wl,relro -Wl,-z -Wl,now -fPIC -shared -rdynamic -Wl,-rpath=${CMAKE_CURRENT_BINARY_DIR}/lib -o libBITCOIND.so bitcoind-bitcoind.o -L${CMAKE_CURRENT_BINARY_DIR}/lib libbitcoin_server.a libbitcoin_server.a libbitcoin_common.a univalue/.libs/libunivalue.a libbitcoin_util.a libbitcoin_consensus.a crypto/libbitcoin_crypto_base.a crypto/libbitcoin_crypto_sse41.a crypto/libbitcoin_crypto_avx2.a crypto/libbitcoin_crypto_shani.a leveldb/libleveldb.a leveldb/libleveldb_sse42.a leveldb/libmemenv.a secp256k1/.libs/libsecp256k1.a -L/usr/lib/x86_64-linux-gnu -lboost_system -lboost_filesystem -lboost_thread -lpthread -lboost_chrono -ldb_cxx -lcrypto -levent_pthreads -levent -levent -pthread
        COMMAND g++ -std=c++11 -O0 -g3 -ftrapv -fstack-reuse=none -Wstack-protector -fstack-protector-all -Wall -Wextra -Wformat -Wvla -Wswitch -Wformat-security -Wredundant-decls -Wno-unused-parameter -Wno-implicit-fallthrough -fPIC -pthread -Wl,-z -Wl,relro -Wl,-z -Wl,now -fPIC -shared -rdynamic -Wl,-rpath=${CMAKE_CURRENT_BINARY_DIR}/lib -o libBITCOIND.so bitcoind-bitcoind.o -L${CMAKE_CURRENT_BINARY_DIR}/lib libbitcoin_server.a libbitcoin_wallet.a libbitcoin_server.a libbitcoin_common.a univalue/.libs/libunivalue.a libbitcoin_util.a libbitcoin_consensus.a crypto/libbitcoin_crypto_base.a crypto/libbitcoin_crypto_sse41.a crypto/libbitcoin_crypto_avx2.a crypto/libbitcoin_crypto_shani.a leveldb/libleveldb.a leveldb/libleveldb_sse42.a leveldb/libmemenv.a secp256k1/.libs/libsecp256k1.a -L/usr/lib/x86_64-linux-gnu -lboost_system -lboost_filesystem -lboost_thread -lpthread -lboost_chrono ${CMAKE_CURRENT_BINARY_DIR}/lib/libdb_cxx-4.8.so -lcrypto -levent_pthreads -levent -levent -pthread  -D__LIBTOOL_IS_A_FOOL__   -D__LIBTOOL_IS_A_FOOL__
        COMMENT "Additional build step for building libBITCOIND.so"
        DEPENDEES build
        DEPENDERS install
        WORKING_DIRECTORY <SOURCE_DIR>/src
        )

ExternalProject_Add_StepDependencies(bitcoin-with-wallet patch openssl-2)
ExternalProject_Add_StepDependencies(bitcoin-with-wallet patch bdb-4.8)
#add_dependencies(bitcoin-with-wallet openssl-2)
#add_dependencies(bitcoin-with-wallet bdb-4.8)

## enable CTEST
enable_testing()

add_custom_target(shadow-bitcoin-with-wallet-prepare ALL
        COMMAND rm -rf data/bcdnode1
        COMMAND rm -rf data/bcdnode1
        COMMAND mkdir -p data/bcdnode1
        COMMAND mkdir -p data/bcdnode2
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME shadow-bitcoin-with-wallet
        COMMAND shadow example.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set_tests_properties(shadow-bitcoin-with-wallet PROPERTIES DEPENDS shadow-bitcoin-with-wallet-prepare)
