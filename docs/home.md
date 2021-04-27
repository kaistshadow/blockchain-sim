Welcome to the blockchain-sim (BLEEP) wiki!

# Overview
 BLEEP (BLockchain Emulation and Evaluation Platform) is blockchain development and testing platform with hierarchical layers consisting of modular & configurable components. 
 BLEEP enables blockchain developers to test and evaluate their blockchain in a scalable fashion. BLEEP suggests pluggable interfaces so that the blockchain developers can seamlessly plug their blockchain into our testing environments.

 We will offer throughput/latency evaluation for consensus algorithm, vulnerability analysis for P2P protocol (such as well known Eclipse attack or a state-of-the-art P2P attack called EREBUS), and many more in the future.

BLEEP 프로젝트는 크게 3개의 컴포넌트로 구성되며, 이를 위해 3개의 subdirectory가 존재한다.

BLEEPlib (Blockchain Library) : 
* [https://github.com/kaistshadow/blockchain-sim/tree/master/BLEEPlib](https://github.com/kaistshadow/blockchain-sim/tree/master/BLEEPlib)

shadow (Blockchain Emulator/Simulator) : 
* [https://github.com/kaistshadow/blockchain-sim/tree/master/shadow](https://github.com/kaistshadow/shadow)

BLEEPeval (Blockchain Evaluation) : 
* [https://github.com/kaistshadow/blockchain-sim/tree/master/BLEEPeval](https://github.com/kaistshadow/blockchain-sim/tree/master/BLEEPeval)


# Blockchain development platform

 When you are using a BLEEP, you only need to focus on the interesting components of the blockchain instead of developing a blockchain from the scratch. (For example, you can develop your Consensus algorithm without considering the remaining parts such as socket I/O, P2P membership, etc.) User can seamlessly leverage other parts of the blockchain offered by BLEEP. In this way, BLEEP offers a standardized way of developing blockchain components and comparing them with different implementations.

BLEEP 은 블록체인의 구현을 위해 BLEEP library(BLEEPlib)를 제공하며 아래와 같은 핵심 모듈들을 제공하는 것을 목적으로 한다. 
* 블록체인 네트워크에서 이웃 Peer 정보들을 공유하고 관리하기 위한 멤버쉽 프로토콜 모듈
* Transaction을 추상화하여 생성하는 모듈
* Transaction을 블록체인 네트워크에 무작위로 전파하여 공유하는 P2P gossip 프로토콜 모듈
* Transaction을 Block 단위로 연결시키고 permissionless 네트워크 상에서 합의하기 위해 PoW 알고리즘을 동작시키는 PoW기반 합의 모듈
* PoS기반 합의 모듈, PBFT 기반 합의 모듈

위의 모듈들을 이용해, BLEEP user 는 블록체인 어플리케이션을 쉽게 개발할 수 있으며, BLEEP 이 제공하는 테스트 프레임워크에서 쉽게 구현물을 동작시키고 결과를 시각화하여 확인할 수 있다.

# Blockchain evaluation(testing) platform 

BLEEP은 임의의 블록체인 프로그램을 테스트할 수 있는 테스트 프레임워크를 제공한다. (이는 위에서 설명한 블록체인 개발 프레임워크와는 독립적이다.)
BLEEP을 이용해, 블록체인 개발자들은 자신들이 개발한 혹은 사용하고자하는 임의의 블록체인에 대하여 블록체인의 성능, 안전성, 보안성 등을 테스트할 수 있다. 
BLEEP의 블록체인 테스트 프레임워크는 아래의 기능들을 제공하는 것을 목적으로 한다.
* 1\~10개의 노드로 이루어진 소규모 블록체인 네트워크부터 1000\~10000개의 노드로 이루어진 대규모 네트워크까지 하나의 플랫폼 상에서 동작시키는 기능
* 네트워크 노드 failure, link failure, 네트워크 파티셔닝 등 네트워크 레벨에서의 다양한 실패 상황을 injection하는 기능
* 블록체인이 처리할 수 있는 transaction들의 throughput (TPS) 및 latency에 대한 측정 기능
* 블록체인이 기반하는 P2P 모듈에 대한 취약성 검증 기능 


# Compile & Run
BLEEP을 이용하기 위해서는 우선 아래의 명령어로 컴파일을 한다.
```bash
git clone https://github.com/kaistshadow/blockchain-sim
git submodule update --init
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j4
```

아래의 명령어로 설치한다. (프로젝트 루트 디렉토리의 Install 디렉토리로 필요한 파일들이 설치된다.)
```bash
make install
```

제대로 컴파일 및 설치가 되었는지 확인하기 위해 테스트 파일들을 빌드하고 실행해 볼 수 있다.
```bash
cmake -DTEST_OPT=ON -DCMAKE_BUILD_TYPE=Release ..
make
make test
```

SybilAPI를 이용해 구현된 bitcoin에 대한 EREBUS 공격실험은 아래의 명령어를 통해 실행해볼 수 있다.
```bash
cd ../BLEEPeval/sybiltest-app/bitcoin
../../../Install/bin/shadow test-BitcoinP2P.xml
```

실험 결과는 shadow.data 폴더에 저장되는 노드들의 stdout 및 stderr 출력값을 통해 알 수 있다. 


# Easy installation
BLEEP은 빠른 설치를 위해 파이썬 스크립트를 제공한다.  
아래의 명령어로 저장소를 다운로드받고 필요한 모든 컴포넌트들을 쉽게 설치할 수 있다.

```bash
git clone https://github.com/kaistshadow/blockchain-sim
cd blockchain-sim
python setup.py --all
```

또한 아래와 같이 테스트를 할 수 있다.
```bash
python setup.py --test
```

우리는 Ubuntu 18.04 LTS에서 BLEEP이 제대로 동작하는 것을 확인하였다.