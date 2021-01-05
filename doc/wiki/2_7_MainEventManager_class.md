 _MainEventManager_ class는 asynchronous event 핸들링 메커니즘을 구현한다. 네트워크 어플리케이션에서는 다른 peer 가 자신과의 연결을 끊는다거나, 새롭게 자신에게 메시지를 보내는 등의 이벤트가 예상하지 못한 타이밍에(즉, asynchronous하게) 발생할 수 있는 환경이다. 
 또한, 블록체인 어플리케이션에서는 mining 등을 효율적으로 구현하기 위해 asynchronous 하게 결과를 리턴하는 thread 나 타이머를 동작시킬 수도 있다. 우리는, asynchronous 하게 동작하는 다양한 상황을 지원하기 위해 asynchronous event 핸들링 메커니즘을 구현하고자 하였다.

 asynchronous event 핸들링 메커니즘을 구현하기 위해서는, 이벤트가 발생했을 때, user 에게 이벤트가 발생했음을 알려주고, 어떤 이벤트가 발생했는지에 대해 정보를 전달해주는 메커니즘이 필요하게 된다. _MainEventManager_ 는 이를 위해 3개의 API, 즉, _Wait_, _ExistAsyncEvent_, _PopAsyncEvent_ 들을 제공함으로써, asynchronous 한 이벤트가 발생한 시점에, 해당 event 를 user가 즉시 받을 수 있도록 도와준다. 예를 들어 네트워크 상에서 asynchronous 하게 이벤트(새로운 연결 생성, 새로운 메세지 도착 등)들이 발생했을 때, 해당 event 들은 _MainEventManger_ 가 관리하는 event queue 에 쌓이게 되고, user는 위의 API 들을 이용해 이를 확인할 수 있다. 

 _MainEventManager_ 는 asynchronous event queue 에 새로운 이벤트를 push 할 수 있도록 _PushAsyncEvent_ 라는 API 도 제공한다. 다양한 모듈들이 이 API 를 이용해 asynchronous event 를 push 할 수 있다. 예를 들어 _BasicNetworkModule_, _TxGeneratorModule_ 과 같은 모듈들이 asynchronous 이벤트들을 생성하고 _PushAsyncEvent_ API 를 사용해 event 를 push 한다. ( _BasicNetworkModule_ class 에 대한 관련 내용은 [2.9](https://github.com/kaistshadow/blockchain-sim/wiki/2.9-BasicNetworkModule-class)를 참고하도록 한다. _TxGeneratorModule_ class 는 [2.8](https://github.com/kaistshadow/blockchain-sim/wiki/2.8-TxGeneratorModule-class) 을 참고하도록 한다. )

아래는 _MainEventManager_ class가 제공하는 API 들의 list이다.

## API for asynchronous event handling

### _Wait_ API
```c++
void Wait()
```

_Wait_ API 는 asynchronous event가 발생할 때까지 기다리는 함수 API이다. user가 이 API를 호출하면 event가 발생할 때까지 blocking이 되며, asynchronous event가 발생한 순간 (즉, asynchronous event queue 에 새로운 event 가 들어온 순간), API가 return을 하게 된다. 현재 구현에서 이 _Wait_ API는 매우 중요하다. _MainEventManager_ class가 별도의 worker thread를 사용하지 않고 동작하기 때문에, user로부터 요청받은 asynchronous API(_UnicastMessage_ 등)의 동작을 내부적으로 처리하기 위해서는 user가 사용하는 thread를 빌려 동작해야하기 때문이다. user는 _Wait_ API를 이용해서 _MainEventManager_ class에게 자신의 thread를 빌려주는 역할을 한다고 생각할 수 있다. 이러한 구조는 다소 비직관적이므로 향후에 _MainEventManager_ 가 자체적으로 별도의 thread를 동작시키도록 개선이 이루어질 예정이다. 하지만 그 전까지는, 무조건 이 _Wait_ API를 사용해야만 _MainEventManager_ class가 내부적으로 등록된 watcher 들을 동작시킴으로서, API들을 처리하고 asynchronous event들을 발생시킬 수 있다.

### _ExistAsyncEvent_ API
```c++
bool ExistAsyncEvent()
```
**return value**: 현재 pending 상태의 asynchronous event 가 존재하는지를 나타내는 boolean 값

현재 아직 처리하지 않은 asynchronous event가 존재하는지를 확인할 수 있는 API이다. 


### _PopAsyncEvent_ API
```c++
AsyncEvent PopAsyncEvent()
```
**return value**: event queue의 head에 존재하고 있는 event (즉, 가장 먼저 발생한 event) 를 리턴

asynchronous event queue 에서 가장 오래된 event 를 pop 하고, 해당 event 를 리턴한다.

### _PushAsyncEvent_ API
```c++
void PushAsyncEvent(AsyncEvent event)
```
asynchronous event queue 에서 새로운 event 를 push 한다. 네트워크 모듈, consensus 모듈 등과 같은 별도의 모듈에서 새로운 이벤트가 발생했을 때, 사용하게 된다. 

## API for _AsyncEvent_ class

다음으로, asynchronous event 를 추상화한 _AsyncEvent_ class 의 API 는 아래와 같다. User는 _MainEventManager_ 의 API 를 통해 받은 _AsyncEvent_ object 에 대해 아래의 API를 호출함으로써, 원하는 동작을 수행할 수 있다.

### _GetType_ API
```c++
AsyncEventEnum GetType()
```
**return value**: 발생한 asynchronous event 의 type

해당 asynchronous event가 어떤 타입인지 확인할 수 있는 API이다. 

### _GetData_ API
```c++
AsyncEventDataManager& GetAsyncEventDataManager()
```
**return value**: 발생한 asynchronous event 의 data를 저장하고 있는 manager 를 return

발생한 asynchronous event의 data를 접근할 수 있도록 해주는 API이다. 
_AsyncEventDataManager_ 에 대한 reference 변수를 받아와서, event type에 따라 알맞은 함수를 호출하여 data를 가져올 수 있다.
예를 들어, _RecvMessage_ event가 발생했을 경우, _AsyncEventDataManager_ 의 _GetReceivedMsg_ 함수를 호출하면, 받은 Message에 대한 포인터를 리턴한다. 
