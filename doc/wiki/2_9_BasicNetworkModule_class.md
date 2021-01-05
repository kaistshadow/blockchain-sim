_BasicNetworkModule_ class 는 네트워크를 구성하는 peer 간의 통신을 구현하기 위해 필요한 API들을 제공하게 된다. 이를 위해 _BasicNetworkModule_ 가 제공하는 API는 3가지가 있다. peer 간에 연결을 맺고 끊기 위한 _AsyncConnectPeer_, _DisconnectPeer_ API 들과, peer 간에 메시지를 주고 받기 위한 _UnicastMessage_ API 가 그것이다. 


_AsyncConnectPeer_ API 는 _DisconnectPeer_ API나 _UnicastMessage_ API와는 다르게 해당 API 가 어떻게 처리되었는지를 asynchronous 하게 결과를 받아 볼수 있도록 asynchronous event를 trigger하게 된다. User는 [_MainEventManager_](https://github.com/kaistshadow/blockchain-sim/wiki/2.7-MainEventManager-class) 의 asynchronous event 핸들링 API를 통해 _AsyncConnectPeer_ API의 처리 결과를 asynchronous하게 확인할 수 있다. 

아래는 _AsyncConnectPeer_ class가 제공하는 API 및 asynchronous event들의 list이다.

## API for network functionality

### Constructor
```
BasicNetworkModule(std::string myPeerId);
```
**myPeerId**: 자신의 peer id

_BasicNetworkModule_ 를 생성하기 위해 사용하는 생성자 함수이며, 무조건 자신의 peer id 를 인자로 넘겨주어야만 한다. 즉, _BasicNetworkModule_ class는 생성 순간에 자기 자신의 peer id 를 세팅하게 된다.

### _AsyncConnectPeer_ API
```c++
bool AsyncConnectPeer(PeerId id, double time = 0);
```
**id**: 연결하고자 하는 peer의 id  
**time**: **(optional)** 연결하기까지 기다릴 시간  
**return value**: API 함수 결과

다른 peer 로의 연결을 위해 제공되는 API 이다. 연결하고자하는 이웃 peer의 id를 기본 인자로 넣어야 하며, 연결을 바로 시도할 것인지, 아니면 조금 기다린 이후에 시도할 지를 _time_ 인자를 통해 설정할 수 있다. 만일 연결하고자하는 이웃 peer가 이미 valid한 연결이 이루어진 peer였다면 굳이 연결을 시도할 필요가 없으므로, 새로운 연결을 시도하지 않고 false 를 return 한다. 

이 API는 asynchronous 하게 동작하는 API이다. 즉, API는 곧바로 return을 하지만, _BasicNetworkModule_ 는 내부적으로 API가 처리해주어야 하는 일들을 asynchronous하고 user-transparent하게 수행하게 된다. (개발자노트 : 단, 현재 _BasicNetworkModule_ 의 API 들은 일반적인 의미의 asynchronous API 들과는 달리 그 동작이 완전히 user-transparent하지는 않다. 즉, _MainEventManager_ 의 _Wait_ API를 별도로 호출해주어야만 API 처리가 이루어진다.)

이 API는 asynchronous한 event를 발생시키는 API 이다. 즉, 연결 시도 결과에 따라 만일 연결이 무사히 이루어졌다면 _CompleteAsyncConnectPeer_ event를 발생시키고, 연결이 실패했다면 _ErrorAsyncConnectPeer_ event를 발생시킨다.

### _DisconnectPeer_ API
```c++
bool DisconnectPeer(PeerId id);
```
**id**: 연결을 끊고자하는 peer의 id  
**return value**: API 함수 결과

인자로 준 peer에 대한 네트워크 연결을 끊어주는 API이다. true가 리턴된 경우, 해당 연결이 무사히 끊어졌음을 의미한다. 
끊어진 연결의 반대편 peer에서는 asynchronous event 인 _PeerDisconnected_ 가 발생하게 된다. 만일 인자로 들어온 peer에 대한 valid한 연결이 존재하지 않았다면, false를 리턴한다.

### _UnicastMesage_ API
```c++
bool UnicastMessage(PeerId dest, std::shared_ptr<Message> message);
```
**dest**: message를 전송하고자 하는 destination peer의 id  
**message**: 전달하고자 하는 message에 대한 포인터  
**return value**: API 함수 결과  

네트워크에서 이웃 peer에게 메시지를 전송하기 위한 API이다. 메시지를 가르키는 포인터와, destination 의 peer id를 인자로 넣어주면, 메시지 전송이 이루어지게 된다. 만일 **dest** 에 대한 네트워크 연결이 존재하지 않는다면, false 를 리턴하게 된다. 이러한 경우, user는 해당 peer에 대해 _AsyncConnectPeer_ API를 먼저 실행해주어야 한다. 네트워크 전송은 TCP socket을 통해 이루어지며, libev에 의해 socket이 사용가능할 때마다 메시지를 알아서 전송해주게 된다. 따라서 아무리 큰 사이즈의 메시지를 전송 요청하더라도, API는 곧바로 return이 되며, 실제 메시지 전송은 user-transparent하게 내부적으로 처리된다. 메시지가 정말 destination에 제대로 전달이 되었는지는 destination node에서 recvMessage event가 호출되었는지를 확인함으로써 검증할 수 있다. 

이 API는 asynchronous 하게 동작하는 API이다. 즉, API는 곧바로 return을 하지만, _BasicNetworkModule_ 는 내부적으로 API가 처리해주어야 하는 일들을 asynchronous하고 user-transparent하게 수행하게 된다. (_MainEventManager_ 의 _Wait_ API 호출은 필요함)


## List of asynchronous event 

### _CompleteAsyncConnectPeer_ event
_AsyncConnectPeer_ API에 의해 발생하는 event이다. 해당 API가 무사히 이웃노드와의 연결을 완료했을 때 발생한다.
이 이벤트는 아래와 같은 event data를 포함하고 있다.
```
PeerId _connectedPeerId;
int _connectedSocketFD;
```
**_connectedPeerId** : 연결된 이웃노드의 peer id를 의미한다.  
**_connectedSocketFD** : 연결된 이웃노드와의 data통신을 위해 내부적으로 사용하게 된 socket의 file descriptor를 의미한다.  

### _ErrorAsyncConnectPeer_ event
_AsyncConnectPeer_ API가 이웃노드와의 연결에 실패했을 때 발생한다.
아래와 같은 event data를 포함하고 있다.
```
PeerId _refusedPeerId;
int _error;
std::string _errorMsg;
```
**_refusedPeerId** : 연결시도를 했으나 실패한 이웃노드의 peer id를 의미한다.  
**_error** : 연결 실패하는 과정에서 발생한 err 를 의미한다.  
**_errorMsg** : 연결 실패한 이유를 string으로 저장.  

### _NewPeerConnected_ event
이웃노드의 요청에 의해 새로운 연결이 이루어졌을 때 발생하는 event이다. 
아래와 같은 event data를 포함하고 있다.
```
std::shared_ptr<PeerId> _newConnectedPeerId;
```
**_newConnectedPeerId** : 연결된 이웃노드의 peer id를 의미한다.  

### _PeerDisconnected_ event
이웃노드의 요청(혹은 외부요인)에 의해 이웃노드와의 연결이 끊어졌을 때 발생하는 event이다. 
아래와 같은 event data를 포함하고 있다.
```
std::shared_ptr<PeerId> _disconnectedPeerId;
```
**_disconnectedPeerId** : 연결이 끊긴 이웃노드의 peer id를 의미한다.  

### _RecvMessage_ event
이웃노드로부터 새로운 메시지를 받았을 때 발생하는 event이다. 
아래와 같은 event data를 포함하고 있다.
```
std::shared_ptr<Message> _receivedMsg;
```
**_disconnectedPeerId** : 받은 Message를 의미한다.
