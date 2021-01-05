현재 BLEEP 은 블록체인의 구현을 위해 아래와 같은 핵심 모듈들을 통해 API들을 제공하고 있다.

* Transaction 을 추상화한 [_Transaction_ class](https://github.com/kaistshadow/blockchain-sim/wiki/2.1-Transaction-class), Transaction 을 하나의 pool 로 관리하기 위한 [_TxPool_ class](https://github.com/kaistshadow/blockchain-sim/wiki/2.2-TxPool-class)
* Block 을 구현한 [_Block_ class](https://github.com/kaistshadow/blockchain-sim/wiki/2.3-Block-class), Block을 Blockchain 형태로 관리하기 위한 [_LedgerManager_ class](https://github.com/kaistshadow/blockchain-sim/wiki/2.4-LedgerManager-class)
* 네트워크를 구성하는 peer 들의 identification 을 위한 [_PeerId_ class](https://github.com/kaistshadow/blockchain-sim/wiki/2.5-PeerId-class)
* 네트워크를 통해 전송하는 메시지를 추상화 시킨 [_Message_ class](https://github.com/kaistshadow/blockchain-sim/wiki/2.6-Message-class)
* Asynchronous한 event 들을 다루기 위한 [_MainEventManager_ class](https://github.com/kaistshadow/blockchain-sim/wiki/2.7-MainEventManager-class)
* 랜덤 Transaction 을 asynchronous 하게 생성하기 위한 [_TxGeneratorModule_ class](https://github.com/kaistshadow/blockchain-sim/wiki/2.8-TxGeneratorModule-class)
* 네트워크를 구성하는 peer 간의 연결 및 통신을 위한 [_BasicNetworkModule_ class](https://github.com/kaistshadow/blockchain-sim/wiki/2.9-BasicNetworkModule-class), [_RandomGossipNetworkModule_ class](https://github.com/kaistshadow/blockchain-sim/wiki/2.b-RandomGossipNetworkModule-class)
* POW 의 핵심 API를 위한 [_POWModule_ class](https://github.com/kaistshadow/blockchain-sim/wiki/2.a-POWModule-class)
