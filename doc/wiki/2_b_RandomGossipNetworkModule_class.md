_RandomGossipNetworkModule_ class 는 _BasicNetworkModule_ class 를 참고하여 별도의 모듈로 구현되었다. 이 class는 Random Gossip을 구현하기 위한 API들을 제공하게 된다. 

 제공되는 API는 8가지가 있다. 자신의 PeerId를 확인하는 _GetMyPeerId_ , 메시지의 중복 관리을 위하여, MessageSet에 신규 메시지를 추가하는 _InsertMessageSet_ , 메시지가 MessageSet에 존재하는지 여부를 확인하는 _ExistMessage_ , Peer간의 연결을 맺고 끊기 위한 _AsyncConnectPeer_ , _AsyncConnectPeers_ , _DisconnextPeer_ , Peer들간의  메시지 전송을 위한 _UnicastMessage_ , _MulticastMessage_ 가 있다.

## API for network functionality

### Constructor
```
RandomGossipNetworkModule(std::string myPeerId, MainEventManager* worker, int fanOutNum);
```
**myPeerId**: 자신의 peer id  
**fanOutNum**: gossip 메시지 전송시 사용되는 fanOut 수

### _GetMyPeerId_ API
```c++
PeerId GetMyPeerId()
```
### _InsertMessageSet_ API
```c++
bool InsertMessageSet(std::string messageId);
```
**messageId**: 입력하고자 하는 message의 ID  
**return value**: 입력성공: true, 입력실패: false

### _ExistMessage_ API
```c++
bool ExistMessage(std::string messageId);
```
**messageId**: 확인하고자 하는 message의 ID  
**return value**: 있음: true, 없음: false

### _AsyncConnectPeer_ API
```c++
bool AsyncConnectPeer(PeerId id, double time = 0);
```
**id**: 연결하고자 하는 peer의 id  
**time**: **(optional)** 연결하기까지 기다릴 시간  
**return value**: API 함수 결과

### _AsyncConnectPeers_ API
```c++
bool AsyncConnectPeers(std::vector<PeerId, std::allocator<PeerId>> &peerList, int peerNum, double time =     0);
```
**peerList**: 연결가능한 peer id의 List
**peerNum**: 연결하고자 하는 peer의 수
**time**: **(optional)** 연결하기까지 기다릴 시간  
**return value**: API 함수 결과

### _UnicastMesage_ API
```c++
bool UnicastMessage(PeerId dest, std::shared_ptr<Message> message);
```
**dest**: message를 전송하고자 하는 destination peer의 id  
**message**: 전달하고자 하는 message에 대한 포인터  
**return value**: API 함수 결과  

### _MulticastMessage_ API
```c++
bool MulticastMessage(std::shared_ptr<Message> message);
```
**message**: 전달하고자 하는 message에 대한 포인터  
**return value**: API 함수 결과  

