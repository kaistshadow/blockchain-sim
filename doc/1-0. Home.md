Welcome to the blockchain-sim (BLEEP) wiki!

# Overview
 BLEEP (BLockchain Emulation and Evaluation Platform) is blockchain development and testing platform with hierarchical layers consisting of modular & configurable components. 

 When you are using a BLEEP, you only need to focus on the interesting components of the blockchain instead of developing a blockchain from the scratch. (For example, you can develop your Consensus algorithm without considering the remaining parts such as socket I/O, gossiping, etc.) User can seamlessly leverage other parts of the blockchain offered by BLEEP. In this way, BLEEP offers a standardized way of developing blockchain components and comparing them with different implementations.

현재 BLEEP 은 블록체인의 구현을 위해 아래와 같은 핵심 모듈들을 제공하고 있다. 
* Transaction 을 추상화 시킨 Transaction 모듈과, Transaction 을 하나의 pool 로 관리하기 위한 TxPool 모듈
* Block 을 추상화 시킨 Block 모듈과 이를 Blockchain 형태로 관리하기 위한 LedgerManager 모듈
* 네트워크를 구성하는 peer 들의 identification 을 위한 PeerId 모듈 
* 네트워크를 통해 전송하는 메시지를 추상화 시킨 Message 모듈
* 네트워크를 구성하는 peer 간의 통신을 다루기 위한 BasicNetwork모듈(BasicNetworkModule)
* POW 의 핵심 API를 위한 POW모듈(POWModule)
* Asynchronous한 event 들을 다루기 위한 MainEventManager 모듈

위의 모듈들을 이용해, BLEEP user 는 자신만의 블록체인 어플리케이션을 쉽게 개발할 수 있으며, BLEEP 이 제공하는 테스트 프레임워크에서 쉽게 구현물을 동작시키고 결과를 시각화하여 확인할 수 있다.

BLEEP 프로젝트는 크게 4개의 컴포넌트로 구성되며, 이를 위해 4개의 subdirectory가 존재한다.

BLEEPapp (Blockchain Application) :
* [https://github.com/kaistshadow/blockchain-sim/tree/master/BLEEPapp](https://github.com/kaistshadow/blockchain-sim/tree/master/BLEEPapp)

BLEEPlib (Blockchain Library) : 
* [https://github.com/kaistshadow/blockchain-sim/tree/master/BLEEPlib](https://github.com/kaistshadow/blockchain-sim/tree/master/BLEEPlib)

shadow (Blockchain Emulator/Simulator) : 
* [https://github.com/kaistshadow/blockchain-sim/tree/master/shadow](https://github.com/kaistshadow/shadow)

BLEEPeval (Blockchain Evaluation) : 
* [https://github.com/kaistshadow/blockchain-sim/tree/master/BLEEPeval](https://github.com/kaistshadow/blockchain-sim/tree/master/BLEEPeval)



# Easy installation
BLEEP은 빠른 설치를 위해 파이썬 스크립트를 제공한다.  
아래의 명령어로 저장소를 다운로드받고 필요한 모든 컴포넌트들을 쉽게 설치할 수 있다.

```bash
git clone https://github.com/kaistshadow/blockchain-sim
cd blockchain-sim
python setup.py --install
```

또한 아래와 같이 테스트를 할 수 있다.
```bash
python setup.py --test
```

우리는 Ubuntu 16.04 LTS. 와 Ubuntu 18.04 LTS, Ubuntu 19.10에서 BLEEP이 제대로 동작하는 것을 확인하였다.

# Tutorials

Setup and Usage Instructions of BLEEP library : 
* [https://github.com/kaistshadow/blockchain-sim/wiki/1.1-Tutorial-:-Blockchain-application-development](https://github.com/kaistshadow/blockchain-sim/wiki/1.1-Tutorial-:-Blockchain-application-development)

Setup and Usage Instructions of Blockchain emulation : 
* [https://github.com/kaistshadow/blockchain-sim/wiki/1.2-Tutorial-:-Blockchain-emulation](https://github.com/kaistshadow/blockchain-sim/wiki/1.2-Tutorial-:-Blockchain-emulation)

Setup and Usage Instructions of Blockchain evaluation : 
* [https://github.com/kaistshadow/blockchain-sim/wiki/1.3-Tutorial-:-Blockchain-evaluation](https://github.com/kaistshadow/blockchain-sim/wiki/1.3-Tutorial-:-Blockchain-evaluation)

Developing & testing a simple bitcoin using BLEEP: 
* [https://github.com/kaistshadow/blockchain-sim/wiki/1.4-Tutorial-:-Simple-bitcoin-example](https://github.com/kaistshadow/blockchain-sim/wiki/1.4-Tutorial-:-Simple-bitcoin-example)

# API documentation

API documentation of BLEEP library : 
* [https://github.com/kaistshadow/blockchain-sim/wiki/2.-libBLEEP-API](https://github.com/kaistshadow/blockchain-sim/wiki/2.-libBLEEP-API)

