BLEEP에서는 사용자가 간단한 블록체인을 쉽게 개발 할 수 있도록 블록체인에서 사용되는 소켓연결, 피어관리 및 세부 프로토콜에 대한 구현 샘플을 제공한다. 전체 블록체인 구현체를 3개의 프로코톨 계층으로 분류하며, 각 계층의 일반적인 API를 제공함으로써 사용자가 추가하고 싶은 기능을 다른 계층에 영향 없이 구현하기 용이하게 설계되어있다.

# Basic Logic Flow of app. using BLEEP library
(diagram)

(expl.)

# BLEEPlib Module Structure
BLEEPlib의 코드는 다음과 같은 3개의 프로토콜 계층 모듈로 나뉘어져있다.
- 소켓 연결 계층: 특정 도메인 또는 ip에 대한 Connect/Disconnect, Receive, Write 수행. 외부 모듈에 제공되는 API는 다음과 같다.
  - int ConnectSocket(string dest): 연결 대상(dest)에 TCP nonBlocking socket 연결시도 후 매핑된 파일 디스크립터 반환
  - void AbandonConnectSocket(int fd): 주어진 파일 디스크립터에 대응되는 연결 대상에 대한 연결 종료
  - void SendToSocket(int fd, const char* buf, int size): 파일 디스크립터 매핑 대상에 바이트단위 데이터를 전송
  - void DisconnectSocket(int fd): 주어진 파일 디스크립터에 대응되는 연결 대상에 대한 연결 종료
  - void CloseAllListenSocket(): 모든 listen 소켓의 종료
- 피어 연결 계층
  - (expl.)
- 상위 프로토콜 계층
  - (expl.)
