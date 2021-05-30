# 선행지식
- Shadow<br>[Shadow](https://github.com/shadow/shadow)는 unix-type systems에서 동작히는 실제 어플리케이션을 시뮬레이션 해주는 오픈소스 프로그램이다. 여기서 실제 어플리케이션(바이너리)를 shadow환경에서 실행 시키기 위해서는 Shared object 포맷으로 변형해야만 한다. 즉 일반 바이너리 파일인 `target`을 `target.so`로 변형을 해야만 한다.
- Build system <br>
블록체인 프로그램은 대부분 빌드시스템을 이용하여 빌드및 컴파일을 한다. 그렇기에 포팅하고자 하는 블록체인의 빌드시스템에 대한 이해가 없으면 포팅을 할 수 없다. 대부분의 블록체인의 빌드시스템은 [CMake](https://cmake.org/cmake/help/latest/guide/tutorial/index.html), [AutoMake](https://linux.die.net/man/1/automake)를 사용한다. 
<br><br>
# shared object 만들기
### 1. PIC (position independent code) && shared 
PIC 옵션은 리눅스 메모리 보호 기법으로서, shadow환경에서 시뮬레이션 하기 위해서는 target의 모든 object들이 `PIC` 옵션으로 컴파일 되어야한다. 마지막으로 PIC옵션으로 컴파일된 object들을 이용하여 최종 실행파일을 컴파일할 때 `-shared`과 함께 컴파일을 해야만한다. 간단한 예시로 test.cpp파일을 shared object로 포팅을 하려면, `g++ SHARED -fPIC -o test.so test.cpp` 이런식으로 fPIC 옵션과 shared 옵션이 반드시 포함된 상태로 컴파일을 해야만 한다. 

### 2. Blockchain porting
1번에서는 shared object 생성을 위해 반드시 필요한 옵션을 설명을 했다. 이번에는 각종 블록체인(Bitcoin, Ethereum, ...)의 실행파일을 shared object로 포팅을 위한 작업을 정리를 할 것이다. 
1. Build system 확인<br>
자신이 포팅하고 싶은 블록체인의 빌드 시스템을 확인하여, 해당 빌드시스템의 objecte compile 옵션을 PIC로 변경 또는 설정을 해준다. 보통 블록체인 빌드시스템은 CMake와 Automake가 사용된다. <br><br>
2. Binary verbose<br>
블록체인의 실행파일을 컴파일할 때 사용된 라이브러리들을 확인하는 작업이다. 대부분의 블록체인은 `Make` Build tool을 사용한다. `make SHELL='sh -x'` 명령어를 통해서 블록체인의 빌드, 컴파일 과정에서 사용된 명령어들을 확인할 수 있다. 이 작업을 통해 포팅하기 위한 블록체인 실행파일의 컴파일 명령어를 알아둔다. 
<br><br>
3. fPIC 옵션<br>
2번 과정에서 블록체인 binary에 링크된 object들의 컴파일 옵션으로 `PIC`을 변경 또는 추가를 해준다. 이때 Build system을 활용하면 쉽게 object들을 PIC옵션을 추가하여 compile을 할 수 있다. <br><br>
- AutoMake<br>
 automake 빌드시스템을 사용하는 Bitcoin 같은 경우 Makefile.am파일에 object 컴파일 옵션을 컨트롤 할 수 있다. 그렇기에 다음과 같이 fPIC 옵션으로 변경을 해줄 수 있다. <br>`libbitcoin_server_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIC_FLAGS)` <br> 위의 예시인 bitcoin_server_a는 bitcoind를 생성하기 위해 필요한 라이브러리 중 하나로서, 이런식으로 사용된 모든 라이브러리들의 object 컴파일 옵션을 PIC 옵션으로 변경을 하면 된다. 최종적으로 configure build system 명령어에 다음과 같이 fPIC 옵션을 추가를 하면 된다. `./configure --with-pic` <br><br>
 - CMake<br>
 CMake 빌드시스템은 블록체인 실행 바이너리 파일의 target을 확인하고 그 target에 link , compile 정의를 확인한다. ripple을 예로들면 다음은 `rippled`에 링크되는 모든 object의 컴파일 옵션으로 PIC 옵션을 추가해주는 과정이다. <br>`target_compile_options(rippled PRIVATE "-fPIC")` 이렇게 되면 rippled를 컴파일 하기 위해 사용된 object들이 모두 PIC 옵션으로 컴파일 되는 것이다. 즉 CMake 빌드 시스템으로 구현된 블록체인은 binrary의 target을 찾고, 그 target에 연관된 target(libraryt, object)의 dependency를 확인하여 fPIC옵션으로 컴파일 하도록 해야한다. <br><br>
 4. shared object 만들기<br>
 2번 과정을 통해서 블록체인 바이너리 컴파일 명령어를 알 수 있었고, 3번 과정을 통해서 2번 과정에서 컴파일된 object들을 PIC 옵션으로 컴파일되게 하였다. 2번 과정으로 얻은 바이너리 컴파일 명령어에 `-shared` 옵션과 -fPIC 옵션을 추가하여 최종적으로 shared object를 생성한다.

### 3. shadow dependency
porting 작업의 메뉴얼은 2번 처럼 진행을 하면 된다. 하지만 포팅된 블록체인이 shadow와 호환이 맞지 않는 라이브러리를 사용을 한다면 오류가 발생한다. 그렇기에 shadow와 호환이 안된 라이브러리 및 문법을 정리 및 해결책을 정리를 하였다. <br><br>
1. Openssl<br><br>
openssl은 대부분의 os에 default로 설치된 라이브러리며, 최신버젼은 1.1.1이다. 하지만 shadow에서는 openssl1.1.0에 호환이 된다. 대부분의 블록체인은 openssl 라이브러리를 사용을 하고, 실행파일의 링크를 확인해보면 system에 설치된 1.1.1버젼의 openssl library를 가리킨다. 결국 2번 메뉴얼 처럼 포팅을 하더라도 호환에 맞지 않는 라이브러리로 링크가 되면 에러가 발생한다. 
- 해결책 : BLEEP에서는 openssl 1.1.0 버젼을 submodule로 설치를 해주기에, 블록체인을 포팅하는 과정에서 openssl을 사용하는 object, library들이 BLEEP에서 설치한 1.1.0 버젼의 openssl을 사용하도록 해주면 된다. <br>

<br>

2. ZeroMQ<br><br>
Openssl같은 경우 블록체인이 기능하기 위해서 반드시 사용되는 라이브러리라 호환되는 버젼을 맞추는 식으로 까지 추가하여 컴파일을 하였다. 반면 ZeroMQ library는 Openss처럼 반드시 필요한 라이브러리가 아니라, 컴파일 과정에서 다음과 같이 제외 시켜준다.
```
./configure --disable-zmq
```

3. TLS(Thread local storage)<br><br>
shadow에서는 Thread local storage기능을 지원하지 않는다. 만약 Thread local storage가 openssl처럼 블록체인이 기능하기에 반드시 필요한 부분이면, 해당 블록체인은 포팅하기 까다롭다. Bitcoin 같은 경우 Thread local storage를 사용하지만, 다행히도 Thread local storage없이도 기능할 수 있어서 다음 처럼 제외 시켜주면 된다.
```
./configure --disable-threadlocal
```
<br><br>

# TIP
## case 1. 메뉴얼에 맞게 포팅을 하였지만, 오류 발생
- 라이브러리 누락<br>
메뉴얼2-2를 통해 binary verbose(binray compile 명령어)와 메뉴얼3-4를 통해 만들어진 shared object의 공유라이브러리 리스트와 비교하여, 누락된 옵션 또는 라이브러리가 있을 경우 추가를 해준다.<br><br>
- shadow dependency 확인<br>
shared object의 공유라이브러리 리스트를 확인하여, openssl이 BLEEP의 openssl을 사용하는지, zmq를 사용하는지를 확인하여 그렇지 않은 경우들에 대해 수정을 해준다. 다음은 bitcoin의 shared object를 ldd 명령어를 수행하였다. BLEEP에서 설치된 Openssl을 사용하는 것을 알 수 있으며, zmq, tls 등 shadow dependency에 맞게 링크되어 있음을 확인할 수 있다.
```
mong@mong-System-Product-Name:~/dependency/blockchain-sim/tests/regtest/1_bitcoin/3_blockchainApplication$ ldd libBITCOIND_0.19.1DEV.so 
	linux-vdso.so.1 (0x00007fff3d54a000)
	libboost_system.so.1.65.1 => /usr/lib/x86_64-linux-gnu/libboost_system.so.1.65.1 (0x00007f19cd30e000)
	libboost_filesystem.so.1.65.1 => /usr/lib/x86_64-linux-gnu/libboost_filesystem.so.1.65.1 (0x00007f19cd0f4000)
	libboost_thread.so.1.65.1 => /usr/lib/x86_64-linux-gnu/libboost_thread.so.1.65.1 (0x00007f19ccecf000)
	libpthread.so.0 => /lib/x86_64-linux-gnu/libpthread.so.0 (0x00007f19cccb0000)
	libboost_chrono.so.1.65.1 => /usr/lib/x86_64-linux-gnu/libboost_chrono.so.1.65.1 (0x00007f19ccaab000)
	libdb_cxx-4.8.so => /home/mong/dependency/blockchain-sim/Install/bdb_4.8.30/lib/libdb_cxx-4.8.so (0x00007f19cc70a000)
	libcrypto.so.1.1 => /home/mong/dependency/blockchain-sim/Install/openssl_1.1.0h/lib/libcrypto.so.1.1 (0x00007f19cc25b000)
	libevent_pthreads-2.1.so.6 => /usr/lib/x86_64-linux-gnu/libevent_pthreads-2.1.so.6 (0x00007f19cc058000)
	libevent-2.1.so.6 => /usr/lib/x86_64-linux-gnu/libevent-2.1.so.6 (0x00007f19cbe07000)
	libSHADOW_INTERFACE.so => /home/mong/dependency/blockchain-sim/Install/lib/libSHADOW_INTERFACE.so (0x00007f19cbc04000)
	libSHADOW_BITCOIN_INTERFACE.so => /home/mong/dependency/blockchain-sim/Install/lib/libSHADOW_BITCOIN_INTERFACE.so (0x00007f19cb9f4000)
	libstdc++.so.6 => /usr/lib/x86_64-linux-gnu/libstdc++.so.6 (0x00007f19cb66b000)
	libm.so.6 => /lib/x86_64-linux-gnu/libm.so.6 (0x00007f19cb2cd000)
	libgcc_s.so.1 => /lib/x86_64-linux-gnu/libgcc_s.so.1 (0x00007f19cb0b5000)
	libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f19cacc4000)
	/lib64/ld-linux-x86-64.so.2 (0x00007f19cede8000)
	librt.so.1 => /lib/x86_64-linux-gnu/librt.so.1 (0x00007f19caabc000)
	libdl.so.2 => /lib/x86_64-linux-gnu/libdl.so.2 (0x00007f19ca8b8000)
```

# Ex) Bitcoin porting
## 1. Build system 확인
Bitcoin에서는 Automake Build system을 통해 build와 compile을 한다.
## 2. Binary verbose
일반적으로 Bitcoin을 설치하면서, make 명령 시 verbose를 확인할 수 있게 옵션을 추가해준다.
```
./configure --disable-zmq --disable-threadlocals
make SHELL='sh -x' &> bitcoind_verbose.txt
```
Bitcoin이 설치되면 `bitcoind_verbose.txt`에 bitcoind를 만들기 위해 사용된 compile명령어를 알아둔다.
## 3. fPIC옵션
1번 작업에서 Bitcoin이 Automake Build system을 사용한다는 것을 알았으니, Automake에서 object들의 compile 옵션을 어떤식으로 컨트롤하는지 찾아봐야 한다. Bitcoin은 `Makefile.am`에서 object의 compile 옵션을 컨트롤 한다. 그렇기에 해당 파일을 통해 object들에 PIC compile옵션을 다음과 같이 추가해준다.
```
libbitcoin_server_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIC_FLAGS)
```
## 4. shared object 만들기
2번 작업으로 알게된 binary compile 명령어에 `-shared` ,`-fPIC`옵션을 추가하고, openssl의 rpath와 링크를 BLEEP에서 설치한 openssl로 설정을 해준다.
```
g++ -std=c++11 -O0 -g3 -ftrapv -fstack-reuse=none -Wstack-protector -fstack-protector-all -Wall -Wextra -Wformat -Wvla -Wswitch -Wformat-security -Wredundant-decls -Wno-unused-parameter -Wno-implicit-fallthrough -fPIC -pthread -Wl,-z -Wl,relro -Wl,-z -Wl,now -fPIC -shared -rdynamic -Wl,-rpath=${CMAKE_INSTALL_PREFIX}/openssl_1.1.0h/lib -Wl,-rpath=${CMAKE_INSTALL_PREFIX}/bdb_4.8.30/lib -Wl,-rpath=${CMAKE_INSTALL_PREFIX}/lib -o libBITCOIND.so bitcoind-bitcoind.o -L${CMAKE_INSTALL_PREFIX}/openssl_1.1.0h/lib -L${CMAKE_INSTALL_PREFIX}/bdb_4.8.30/lib -L${CMAKE_INSTALL_PREFIX}/lib -L$<TARGET_FILE_DIR:SHADOW_INTERFACE> libbitcoin_server.a libbitcoin_wallet.a libbitcoin_server.a libbitcoin_common.a univalue/.libs/libunivalue.a libbitcoin_util.a libbitcoin_consensus.a crypto/libbitcoin_crypto_base.a crypto/libbitcoin_crypto_sse41.a crypto/libbitcoin_crypto_avx2.a crypto/libbitcoin_crypto_shani.a leveldb/libleveldb.a leveldb/libleveldb_sse42.a leveldb/libmemenv.a secp256k1/.libs/libsecp256k1.a -L/usr/lib/x86_64-linux-gnu -lboost_system -lboost_filesystem -lboost_thread -lpthread -lboost_chrono -ldb_cxx -lcrypto -levent_pthreads -levent -pthread -lSHADOW_INTERFACE -D__LIBTOOL_IS_A_FOOL__

```
