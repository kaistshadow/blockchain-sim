_Message_ class는 네트워크를 통해 peer간에 정보의 교환이 이루어지는 기본 단위이다. 
_Message_ class는 Message를 보내는 peer(_PeerId_ class), 받는 peer(_PeerId_ class), 메시지의 종류(MessageType), 그리고 실제 메시지의 내용을 의미하는 payload string으로 구성되어 있다. 

_Message_ 를 보내는 입장과 받는 입장에서는 각각 아래와 같은 단계를 거치게 되어 있다.

* 보내는 입장
1. 우선 보내고자하는 정보를 string 형태로 serialization 하여 _Message_ 의 payload string에 저장한다.
2. Message의 type, 보내는 peer, 받는 peer를 세팅하여 온전한 _Message_ object를 만든다. 
3. _Message_ 를 serialization 하여 bytestream으로 만든다.
4. bytestream을 socket을 통해 전송한다.

위의 과정에서 3.,4. 과정은 _BasicNetworkModule_ class의 [_UnicastMessage_ API](https://github.com/kaistshadow/blockchain-sim/wiki/2.9-BasicNetworkModule-class#unicastmesage-api)로 구현되어 있다. 1.,2. 과정은 user가 해주어야 하는 부분이다.

* 받는 입장
1. socket이 받은 bytestream을 deserialization 하여 _Message_ object를 만든다.
2. Message의 type을 보고 Message의 payload를 deserialization 하여, object를 만든다.(Transaction, Block 등)

위의 1. 과정은 _BasicNetworkModule_ class 내부에 구현되어 있으며, 이 과정이 끝나면 [_RecvMessage_ event](https://github.com/kaistshadow/blockchain-sim/wiki/2.9-BasicNetworkModule-class#recvmessage-event) 가 발생한다.
2. 과정은 user가 해주어야 하는 부분이다.

_Message_ class는 아래와 같은 API들을 제공하고 있다.

### _GetType_ API
```c++
MessageType GetType() const
```
**return value**: _Message_ 의 type 정보를 리턴한다.

Message가 어느 type의 Message인지를 알려주는 API이다. _MessageType_ 은 std::string type이다. (typedef)
Message의 type은 Message를 생성할 때 생성자를 통해 세팅할 수 있다. 
따라서, User는 자신이 정의하는 네트워크 프로토콜에 따라 임의의 메시지들을 정의해서 사용할 수 있다.
예를 들어, [tutorial](https://github.com/kaistshadow/blockchain-sim/wiki/1.4-Tutorial-:-Simple-bitcoin-example)에서 우리는 newTx라는 메시지 타입을 사용해서 간단한 bitcoin 예제를 구현했고, 이는 새로운 transaction을 전송할 때 사용하는 메시지 타입이다.

### _GetPayload_ API
```c++
std::string GetPayload() const
```
**return value**: _Message_ 가 가지고 있는 payload (string)

_Message_ 가 가지는 payload를 리턴하는 API 함수이다. payload는 _Message_ class에 serialization 된 형태로 저장되며, 따라서 이 API도 string형태로 payload를 리턴한다.

### _GetSource_ API
```c++
PeerId GetSource() const
```
**return value**: _Message_ 를 보낸 peer의 id를 리턴

_Message_ 를 보내는 peer의 id를 리턴하는 API 함수이다. _Message_ class는 _PeerId_ type 형태로 보내는 사람과 받는 사람의 정보를 가지고 있으며, 따라서 이 API도 _PeerId_ type을 리턴한다.

### _GetDest_ API
```c++
PeerId GetDest() const
```
**return value**: _Message_ 를 받는 peer의 id를 리턴

_Message_ 를 받는 peer의 id를 리턴하는 API 함수이다. _Message_ class는 _PeerId_ type 형태로 보내는 사람과 받는 사람의 정보를 가지고 있으며, 따라서 이 API도 _PeerId_ type을 리턴한다.
