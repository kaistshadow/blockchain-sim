# Introduction to BLEEP
블록체인은 여러가지 기술의 융합을 통해 구현되며 각각의 기술이 수많은 경우의 수를 가지기 때문에 동작을 분석 및 검증하기가 어렵다.<br> 
BLEEP을 이용한다면 다양한 블록체인 어플리케이션의 실제 동작을 단일 머신에서 대규모로 테스트할 수 있다.<br> 
또한 제공하는 테스트 프레임워크를 이용해 성능(TPS, Latency) 및 보안(Sybil)과 관련한 다양한 시나리오를 구성하여 테스트 할 수 있다.<br> 

블록체인의 범위는 현재 C/C++기반 블록체인 기반기술을 대상으로 하며, 향후에는 다양한 언어로 구현된 블록체인과<br> 
이를 이용하는 어플리케이션까지 확장하고자 한다.<br>  

- 이를위해 BLEEP은 다음과 같은 3가지의 핵심 목표를 추구하고 있으며,
  - (1) 개발자를 위한 블록체인 모듈형 라이브러리 및 인터페이스 제공
  - (2) 다양한 언어로 구현된 블록체인 바이너리의 동작을 에뮬레이션
  - (3) 블록체인 동작 검증을 위한 기능 및 성능, 취약성 등의 테스팅 환경 제공

- 아래와 같이 4개의 핵심 컴포넌트로 제공하고 있다. 
  - [BLEEPlib (Blockchain Library)] : 
    * [https://github.com/kaistshadow/blockchain-sim/tree/master/BLEEPlib](https://github.com/kaistshadow/blockchain-sim/tree/master/BLEEPlib)

  - shadow (Blockchain Emulator/Simulator) : 
    * [https://github.com/kaistshadow/blockchain-sim/tree/master/shadow](https://github.com/kaistshadow/shadow)

  - BLEEPemul (Blockchain Emulation) : 
    * [https://github.com/kaistshadow/blockchain-sim/tree/master/BLEEPemul](https://github.com/kaistshadow/blockchain-sim/tree/master/BLEEPemul)

  - BLEEPeval (Blockchain Evaluation) : 
    * [https://github.com/kaistshadow/blockchain-sim/tree/master/BLEEPeval](https://github.com/kaistshadow/blockchain-sim/tree/master/BLEEPeval)


- BLEEP은 그림1과 같이 설계 되었고,
  - 블록체인 개발자가 "개발-테스트 및 배포-실행-결과"의 4단계를 통해 실제 블록체인 어플리케이션의 동작을 검증
  - 구현 및 테스팅을 위한 라이브러리 인터페이스와 대시보드 인터페이스
  - 로깅 작업을 통해 결과를 저장 및 출력(시각화 등)


- CMake 빌드 시스템을 이용해 구조화 되어 있으며, 각 폴더의 역할은 아래와 같다.  
  - 네트워크 시뮬레이터 및 바이너리 에뮬레이터
    - `Shadow` : Shadow는 Tor 네트워크의 동작을 시뮬레이션 하기 위한 오픈 프로젝트이며, BLEEP팀은 Shadow를 기반으로 블록체인의 에뮬레이션 환경을 제공
  - 블록체인 노드 Emulation 테스트
    - `BLEEPemul` : 현재 비트코인 노드에 대한 채굴, 트랜잭션, 노드 확장 등을 에뮬레이션할 수 있는 프레임워크를 제공(자동화된 스크립트 코드 제공)
  - 블록체인 노드 Evaluation 테스트
    - `BLEEPeval` : 블록체인의 구현체의 동작을 평가하기 위한 보안성, 성능 등을 테스트할 수 있는 프레임워크를 제공
  - BLEEP 라이브러리 
    - `BLEEPlib` : 블록체인 기반기술 개발 및 테스팅을 위한 라이브러리(P2P 네트워크, 합의 알고리즘 등) 제공
    - `external`
      - 테스팅 대상이되는 블록체인 및 블록체인이 의존하는 커스텀 라이브러리를 제공
    - `testlibs` 
      - 테스팅을 위해 필요한 스크립트 및 라이브러리 코드 제공
    - `interfaces`
      - BLEEP과 Shadow을 커스텀하게 사용 가능하도록 인터페이스를 제공. BLEEP을 설치하면 Install 폴더에 라이브러리 파일로 생성됨.
  - 단위 및 통합 테스트 
    - `tests` 
      - 블록체인의 동작에 대한 단위 및 통합 테스팅하기 위한 테스트케이스를 제공
  - BLEEP 인스톨 파일
    - `Install`
      - BLEEP에서 공통적으로 사용하는 플러그인, 라이브러리, 헤더파일, CMake파일 등을 제공 <br><br>

![그림1. BLEEP Architecture](https://github.com/kaistshadow/blockchain-sim/blob/feature/191/documentation/doc/images/BLEEP_Architecture.png)


# (1) Blockchain development platform

 When you are using a BLEEP, you only need to focus on the interesting components of the blockchain instead of developing a blockchain from the scratch. (For example, you can develop your Consensus algorithm without considering the remaining parts such as socket I/O, P2P membership, etc.) User can seamlessly leverage other parts of the blockchain offered by BLEEP. In this way, BLEEP offers a standardized way of developing blockchain components and comparing them with different implementations.

BLEEP 은 블록체인의 구현을 위해 BLEEP library(BLEEPlib)를 제공하며 아래와 같은 핵심 모듈들을 제공하는 것을 목적으로 한다. 
* 블록체인 네트워크에서 이웃 Peer 정보들을 공유하고 관리하기 위한 멤버쉽 프로토콜 모듈
* Transaction을 추상화하여 생성하는 모듈
* Transaction을 블록체인 네트워크에 무작위로 전파하여 공유하는 P2P gossip 프로토콜 모듈
* Transaction을 Block 단위로 연결시키고 permissionless 네트워크 상에서 합의하기 위해 PoW 알고리즘을 동작시키는 PoW기반 합의 모듈
* PoS기반 합의 모듈, PBFT 기반 합의 모듈

위의 모듈들을 이용해, BLEEP user 는 블록체인 어플리케이션을 쉽게 개발할 수 있으며, BLEEP 이 제공하는 테스트 프레임워크에서 쉽게 구현물을 동작시키고 결과를 시각화하여 확인할 수 있다.

# (2) Blockchain emulation(testing) platform 

BLEEP은 임의의 블록체인 프로그램의 동작을 에뮬레이션할 수 있는 테스트 프레임워크를 제공한다. (이는 위에서 설명한 블록체인 개발 프레임워크와는 독립적이다.)
BLEEP을 이용해, 블록체인 개발자들은 자신들이 개발한 혹은 사용하고자하는 임의의 블록체인에 대하여 블록체인의 채굴, 트랜잭션 생성 및 전파 등을 테스트할 수 있다. 
블록체인 에뮬레이션을 위한 프레임워크는 아래의 기능들을 제공하는 것을 목적으로 한다.
* 1\~10개의 노드로 이루어진 소규모 블록체인 네트워크부터 1000\~10000개의 노드로 이루어진 대규모 네트워크까지 하나의 플랫폼 상에서 동작시키는 기능
* 블록체인(비트코인) 노드에게 채굴 등의 메시지를 보내는 RPC 노드 지원 
* 블록체인(비트코인) 노드의 채굴, 트랜잭션 등의 동작을 모니터링하는 노드 지원
* 블록체인(비트코인) 에뮬레이션을 동작을 지원(노드 개수, 시뮬레이션 시간, 마이닝 방법, 난이도, 트랜잭션 생성 유무 등)하는 스크립트 코드 지원 

# (3) Blockchain evaluation(testing) platform 

BLEEP은 임의의 블록체인 프로그램의 동작을 평가하기 위한 테스트 프레임워크를 제공한다. (이는 위에서 설명한 블록체인 개발 프레임워크와는 독립적이다.)
BLEEP을 이용해, 블록체인 개발자들은 자신들이 개발한 혹은 사용하고자하는 임의의 블록체인에 대하여 블록체인의 성능, 안전성, 보안성 등을 테스트할 수 있다. 
블록체인 평가를 위한 프레임워크는 아래의 기능들을 제공하는 것을 목적으로 한다.
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