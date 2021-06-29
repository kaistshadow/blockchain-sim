BLEEP에서는 사용자가 간단한 블록체인을 쉽게 개발 할 수 있도록 블록체인에서 사용되는 소켓연결, 피어관리 및 세부 프로토콜에 대한 구현 샘플을 제공한다. 전체 블록체인 구현체를 3개의 프로코톨 계층으로 분류하며, 각 계층의 일반적인 API를 제공함으로써 사용자가 추가하고 싶은 기능을 다른 계층에 영향 없이 구현하기 용이하게 설계되어있다.

# Basic Logic Flow of app. using BLEEP library
(diagram)

(expl.)

# BLEEPlib Module Structure
BLEEPlib의 코드는 다음과 같은 3개의 프로토콜 계층 모듈로 나뉘어져있다.
- 소켓 연결 계층: 특정 도메인 또는 ip에 대한 TCP nonblocking 로직 처리.
  -  특정 도메인 또는 ip에 대한 Connect/Disconnect, Receive, Write API 제공
- 피어 연결 계층: Gossip 프로토콜 및 Node Discovery 등의 Application Layer에서 제어하는 네트워크 로직에 대한 처리를 수행.
  - Blockchain 노드의 Peer 관리
  - 일정 주기마다 수행되는 Ping/Pong, Node Discovery, Peer Update 로직 처리
  - 외부 모듈에 특정 피어에 대한 연결, 연결 종료, 메세지 전파 관련 API 제공
- 상위 프로토콜 계층
  - (expl.)
