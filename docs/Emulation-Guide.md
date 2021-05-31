# Emulation porting guide
[Shadow](https://github.com/shadow/shadow)는 unix-type systems에서 동작히는 실제 어플리케이션을 시뮬레이션 해주는 오픈소스 프로그램이다. 그렇기에 C,C++기반의 어플리케이션만 시뮬레이션이 가능하다. 여기서 실제 어플리케이션(바이너리)을 shadow환경에서 시뮬레이션하기 위해서는 시뮬레이션할 바이너리 실행파일을 Shared object 포맷으로 변형해야 한다.

# Prerequisites
- 시뮬레이션할 어플리케이션은 gcc또는 g++컴파일러로 컴파일된 어플리케이션이거나, CMake또는 Automake로 빌드및 컴파일된 어플리케이션이어야 한다.
- 시뮬레이션할 어플리케이션이 openssl 라이브러리를 사용한다면, 1.1.0 버젼의 openssl을 사용해야한다. 
- Shadow에서 지원하지 못하거나 구현되지 않은 부분은 시뮬레이션할 수 없음
    - TLS(Thread local storage)를 사용하면 안된다.
    - Spin_lock을 사용하면 안된다.
    - pthread관련 함수인 pthread_mutexattr_setpshared(), pthread_mutexattr_setrobust()를 사용하면 안된다.
    - setsocket()함수 사용 시, Shadow에서는 "127.0.0.0" 주소 값을 처리하는 프로세스가 구현되지 않아 사용하면 안된다.


# Porting manual
### 1. PIC (position independent code) && shared 
Shadow환경에서 시뮬레이션 하기 위해서는 시뮬레이션 하기 위한 어플리케이션 target의 모든 object들이 `PIC` 옵션으로 컴파일 되어야한다. 마지막으로 PIC옵션으로 컴파일된 object들을 이용하여 최종 실행파일을 컴파일할 때 `-shared` 옵션과 함께 컴파일을 해야만한다.

### 2. Blockchain porting

1. Binary verbose<br>
대부분의 블록체인은 `Make` Build tool을 사용한다. `make SHELL='sh -x'` 명령어를 통해서 블록체인의 빌드, 컴파일 과정에서 사용된 명령어들을 확인할 수 있다. 이 작업을 통해 포팅하기 위한 블록체인 실행파일의 컴파일 명령어를 알 수 있다. 
<br><br>

2. fPIC 옵션<br>
시뮬레이션할 블록체인 실행파일을 컴파일하는 과정에서 사용된 라이브러리들의 모든 object를 컴파일할 때 `fPIC` 컴파일 옵션을 추가해야한다. 이는 해당 블록체인의 빌드시스템을 활용하면 효율적으로 `fPIC` 컴파일 옵션을 추가해서 컴파일할 수 있다. 블록체인에서 빌드 시스템은 보통 Automake, CMake를 사용한다.
<br>

- AutoMake<br>
AutoMake 빌드 시스템을 사용하는 블록체인을 살펴보면, Makefile.am파일이 존재한다. Makefile.am 파일은 최상위 디렉토리와 하부 디렉토리에 존재하는데, 소스코드가 존재하는 Makefile.am파일에는 각 라이브러리의 object 컴파일 옵션을 컨트롤 할 수 있다. 다음처럼 Makefile.am의 라이브러리  컴파일 옵션 정의를 하여서 `fPIC` 컴파일 옵션을 추가할 수 있다.

```
...
PIC_FLAGS = "-fPIC"
example_library_1_CXXFLAGS = $(AM_CXXFLAGS) ($PIC_FLAGS)

                .
                .
                .

example_library_N_CXXFLAGS = $(AM_CXXFLAGS) ($PIC_FLAGS)
...
```

<br>

 - CMake<br>
CMake 빌드 시스템을 사용하는 블록체인을 살펴보면, 디렉토리 마다 CMakeLists.txt파일이 존재한다. 그 중 블록체인 바이너리에 대한 target이 정의가된 CMakeLists.txt를 찾고, target의 compile시 `fPIC`옵션 값을 추가하여 정의한다.


```
...

add_execute(target_blockchain)

target_compile_options(target_blockchain "-fPIC") 

set(target_blockchain 
example_library_1
        .
        .
        .
example_library_N)

add_library(example_library_1 ...)
            
            .   
            .
            .

add_library(example_library_N ...)

...

```

3. Setting shadow dependency <br>
Shadow에서는 Opeenssl library 버젼 호환 문제가 있다. 그렇기에 Shadow와 호환이 맞는 버젼인 1.1.0 버젼의 Openssl를 사용해야 한다. Openssl 1.1.0버젼은 BLEEP설치 시 같이 설치가 되기에, BLEEP에서 설치한 Openssl 라이브러리를 Openssl이 사용는 경우마다 링크를 해주어 사용하면 된다.<br><br>

4. shared object 만들기<br>
블록체인 바이너리 파일을 만들기 위한 compile 명령어와 fPIC 옵션으로 컴파일된 object들로 구성된 라이브러리를 알고 있으니, 이를 바탕으로 블록체인 바이너리 파일을 만들기 위한 compile 명령어에 object 생성 옵션으로 `-shared`를 추가한다. 마지막으로 fPIC 옵션으로 컴파일된 object로 구성된 라이브러리들을 링크를 해줌으로써 shared object를 생성할 수 있다
<br><br>

# Tools for analysis
1. ldd<br>
ldd명령어는 porting 이후, 검증및 디버깅 시 유용하게 사용이 된다. 이는 라이브러리의 의존성을 확인할 때 사용을 하는 명령어로서, porting과정에서 누락된 라이브러리가 있는지 검증할 때 사용이 가능하며, shadow와 호환이 안되는 라이브러리와 링크가 되었는지 디버깅 과정에서 또한 활용할 수 있다. <br>

![image](https://user-images.githubusercontent.com/38249713/120159274-f8e5e700-c22f-11eb-9b7a-34cc716db3e5.png)

2. objdump<br>
objdump명령어는 리눅스에서 object 파일의 정보를 볼 때 사용을 한다. porting 이후 object가 `fPIC` 컴파일 옵션으로 제대로 컴파일 되었는지, rpath셋팅 등을 검증할 때 활용이 된다.


![image](https://user-images.githubusercontent.com/38249713/120159750-7f022d80-c230-11eb-97d0-d8a9a15a7e4f.png)


# Ex) Bitcoin porting
Porting할 Bitcoin의 버젼은 0.19.1이며, 해당 버젼에서 사용되는 bdb의 버젼은 4.8.3이다. 먼저 Bitcoin에서 필요로 하는 external library인 bdb(4.8,3)와 shadow에 호환을 맞추기 위해 필요한 openssl(1.1.0)을 설치를 한다. 

## 1. Binary verbose
일반적으로 Bitcoin을 설치하면서, make 명령 시 verbose를 확인할 수 있게 옵션을 추가해준다.
```
./configure --disable-zmq --disable-threadlocals
make SHELL='sh -x' &> bitcoind_verbose.txt
```
Bitcoin이 설치되면 `bitcoind_verbose.txt`를 통해 bitcoind를 만들기 위해 사용된 compile명령어는 다음과 같다. 이는 bitcoind의 shared object 생성 시 필요한 정보다.
```
g++ -std=c++11 -O0 -g3 -ftrapv -fstack-reuse=none -Wstack-protector -fstack-protector-all -Wall -Wextra -Wformat -Wvla -Wswitch -Wformat-security -Wredundant-decls -Wno-unused-parameter -Wno-implicit-fallthrough -fPIC -pthread -Wl,-z -Wl,relro -Wl,-z -Wl,now -fPIC -shared -rdynamic -o bitcoind bitcoind-bitcoind.o  libbitcoin_server.a libbitcoin_wallet.a libbitcoin_server.a libbitcoin_common.a univalue/.libs/libunivalue.a libbitcoin_util.a libbitcoin_consensus.a crypto/libbitcoin_crypto_base.a crypto/libbitcoin_crypto_sse41.a crypto/libbitcoin_crypto_avx2.a crypto/libbitcoin_crypto_shani.a leveldb/libleveldb.a leveldb/libleveldb_sse42.a leveldb/libmemenv.a secp256k1/.libs/libsecp256k1.a -L/usr/lib/x86_64-linux-gnu -lboost_system -lboost_filesystem -lboost_thread -lpthread -lboost_chrono -ldb_cxx -lcrypto -levent_pthreads -levent -pthread -lSHADOW_INTERFACE -D__LIBTOOL_IS_A_FOOL__
```

## 2. fPIC옵션
Bitcoin에서 생성되는 라이브러리 및 실행파일의 컴파일 옵션에 대한 정의는 `src/Makefile.am`파일에 위치한다. 각 라이브러리 또는 실행파일 마다 `fPIC` 컴파일 옵션을 의미하는 $(PIC_FLAGS) 플래그 값으로 변경해준다. 
```
libbitcoin_server_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIC_FLAGS)
libbitcoin_zmq_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIC_FLAGS)
libbitcoin_wallet_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIC_FLAGS)
libbitcoin_wallet_tool_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIC_FLAGS)
crypto_libbitcoin_crypto_base_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIC_FLAGS)
crypto_libbitcoin_crypto_sse41_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIC_FLAGS)
crypto_libbitcoin_crypto_avx2_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIC_FLAGS)
crypto_libbitcoin_crypto_shani_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIC_FLAGS)
libbitcoin_consensus_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIC_FLAGS)
libbitcoin_common_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIC_FLAGS)
libbitcoin_util_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIC_FLAGS)
libbitcoin_cli_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIC_FLAGS)
bitcoind_CXXFLAGS = $(AM_CXXFLAGS) $(PIC_FLAGS)
bitcoin_cli_CXXFLAGS = $(AM_CXXFLAGS) $(PIC_FLAGS)
bitcoin_wallet_CXXFLAGS = $(AM_CXXFLAGS) $(PIC_FLAGS)
libbitcoinconsensus_la_CXXFLAGS = $(AM_CXXFLAGS) $(PIC_FLAGS)
leveldb_libleveldb_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIC_FLAGS)
``` 
이후 fPIC 옵션이 적용된 프로세스로 Build및 compile를 다시 해준다. 이때 별도로 설치한 bdb와 PIC플래그 옵션을 추가해준다. compile이 성공하면 fPIC 옵션으로 compile된 object로 구성된 라이브러리가 설치가 성공적으로 되었다.
```
./configure CPPFLAGS='-I${CMAKE_INSTALL_PREFIX}/bdb_4.8.30/include -I${CMAKE_INSTALL_PREFIX}/include LDFLAGS='-L${CMAKE_INSTALL_PREFIX}/bdb_4.8.30/lib --with-pic --disable-bench --disable-threadlocal --disable-zmq
make -j 6
```

## 3. shared object 만들기
Bitcoind 생성 시 사용된 compile 명령어에서 `-fPIC -shared` 옵션과 기존의 output 파일인 `bitcoind`에서 `libBITCOIND.so`로 변경한다. 또한 openssl1.1.0과 bdb4.8.3 라이브러리에 대한 rpath와 link를 설정하고 실행하면 성공적으로 bitcoind의 shared object가 생성이 된다.
```
g++ -std=c++11 -O0 -g3 -ftrapv -fstack-reuse=none -Wstack-protector -fstack-protector-all -Wall -Wextra -Wformat -Wvla -Wswitch -Wformat-security -Wredundant-decls -Wno-unused-parameter -Wno-implicit-fallthrough -fPIC -pthread -Wl,-z -Wl,relro -Wl,-z -Wl,now -fPIC -shared -rdynamic -Wl,-rpath=${CMAKE_INSTALL_PREFIX}/openssl_1.1.0h/lib -Wl,-rpath=${CMAKE_INSTALL_PREFIX}/bdb_4.8.30/lib -Wl,-rpath=${CMAKE_INSTALL_PREFIX}/lib -o libBITCOIND.so bitcoind-bitcoind.o -L${CMAKE_INSTALL_PREFIX}/openssl_1.1.0h/lib -L${CMAKE_INSTALL_PREFIX}/bdb_4.8.30/lib -L${CMAKE_INSTALL_PREFIX}/lib -L$<TARGET_FILE_DIR:SHADOW_INTERFACE> libbitcoin_server.a libbitcoin_wallet.a libbitcoin_server.a libbitcoin_common.a univalue/.libs/libunivalue.a libbitcoin_util.a libbitcoin_consensus.a crypto/libbitcoin_crypto_base.a crypto/libbitcoin_crypto_sse41.a crypto/libbitcoin_crypto_avx2.a crypto/libbitcoin_crypto_shani.a leveldb/libleveldb.a leveldb/libleveldb_sse42.a leveldb/libmemenv.a secp256k1/.libs/libsecp256k1.a -L/usr/lib/x86_64-linux-gnu -lboost_system -lboost_filesystem -lboost_thread -lpthread -lboost_chrono -ldb_cxx -lcrypto -levent_pthreads -levent -pthread -lSHADOW_INTERFACE -D__LIBTOOL_IS_A_FOOL__

```
