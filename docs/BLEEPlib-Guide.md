BLEEP에서는 사용자가 간단한 블록체인을 쉽게 개발 할 수 있도록 블록체인에서 사용되는 소켓연결, 피어관리 및 세부 프로토콜에 대한 구현 샘플을 제공한다. 전체 블록체인 구현체를 3개의 프로코톨 계층으로 분류하며, 각 계층의 일반적인 API를 제공함으로써 사용자가 추가하고 싶은 기능을 다른 계층에 영향 없이 구현하기 용이하게 설계되어있다.

# Basic Logic Flow of Application with BLEEP library
BLEEP Blockchain library를 사용한 플러그인 어플리케이션은 event loop를 이용하여 상황마다 주어진 로직을 처리하는 것을 기본으로 동작한다.
전체적인 흐름은 다음과 같다.
(TODO: diagram: parameter init -> MainEventManager, BL_SocketLayer_API, BL_PeerConnectivityLayer_API, BL_ProtocolLayer_API init -> main event loop[pop event-(cond)> SockAPI / PeerAPI / PrAPI])
1. argument로 전달받은 입력값 파싱
    - BLEEPlib에서 제공하는 argument parser `gArgs`를 이용하여 임의 argument의 존재여부 및 값을 가져올 수 있음
2. 각 하위 모듈 인스턴스(MainEventManager, BL_SocketLayer_API, BL_PeerConnectivityLayer_API, BL_ProtocolLayer_API) 생성
3. `MainEventManager`의 이벤트 루프 처리
    - `MainEventManager`의 루프는 다음과 같이 구현되며, `MainEventManager`는 Wait 함수 내에서 비동기 이벤트를 순차적으로 빼서 담당하는 모듈 인스턴스의 함수를 수행함
    ```
    while(true) {
        MainEventManager::Instance()->Wait(); // main event loop (wait for next event)

        libBLEEP_BL::AsyncEvent event = libBLEEP_BL::MainEventManager::Instance()->PopAsyncEvent();
    }
    ```
    - 각 모듈에서 로직 처리 및 가상 네트워크의 메세지 수신시 비동기 이벤트 `AsyncEvent`를 생성하게 되며, 해당 이벤트는 `MainEventManager`의 처리 큐로 삽입되어 `Wait`함수에서 처리되기를 대기함
    

# BLEEPlib Module Structure
BLEEPlib의 코드는 다음과 같은 3개의 프로토콜 계층 모듈로 나뉘어져있다.
- 소켓 연결 계층(BL_SocketLayer_API): 특정 도메인 또는 ip에 대한 TCP nonblocking 로직 처리.
  -  특정 도메인 또는 ip에 대한 Connect/Disconnect, Receive, Write API 제공
- 피어 연결 계층(BL_PeerConnectivityLayer_API): Gossip 프로토콜 및 Node Discovery 등의 Application Layer에서 제어하는 네트워크 로직에 대한 처리를 수행.
  - Blockchain 노드의 Peer 관리
  - 일정 주기마다 수행되는 Ping/Pong, Node Discovery, Peer Update 로직 처리
  - 외부 모듈에 특정 피어에 대한 연결, 연결 종료, 메세지 전파 관련 API 제공
- 상위 프로토콜 계층(BL_ProtocolLayer_API)
  - (expl.)
