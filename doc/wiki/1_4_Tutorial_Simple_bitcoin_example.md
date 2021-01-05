BLEEP은 간단한 POW consensus를 구현한 simple bitcoin 을 제공한다.

이 tutorial 에서는 이 simple bitcoin에 대한 구현 설명과 함께, 사용자가 어떻게 이 simple bitcoin 을 컴파일하고 동작시킬 수 있는지에 대해 설명하고자 한다.

# Clone BLEEP repository
BLEEP 저장소를 clone하지 않았다면 아래와 같이 다운받도록 한다.
```bash
git clone https://github.com/kaistshadow/blockchain-sim
cd blockchain-sim
```

# Implementation of simple bitcoin 

simple bitcoin 구현 소스가 존재하는 디렉토리로 이동한다
```bash
cd BLEEPapp
```

 node_pow.cpp가 simple bitcoin 을 구현한 소스이다. 또한, 이를 테스트하기 위해 transaction을 생성해서 node 에게 전송하는 기능을 가지는 client(client.cpp)도 구현하였다. 

 node_pow.cpp 를 보면 다음과 같이 libBLEEP 에서 필요한 모듈을 include 하게 된다. 

```c++
#include "mainmodules/MainEventManager.h"
#include "mainmodules/BasicNetworkModule.h"
#include "mainmodules/POWModule.h"

#include "datamodules/Peer.h"
#include "datamodules/Message.h"
#include "datamodules/POWBlock.h"

#include "datamanagermodules/TxPool.h"
#include "datamanagermodules/ListLedgerManager.h"

#include "utility/ArgsManager.h"

using namespace libBLEEP;
```

 가장 중요한 모듈들은 mainmodules 서브디렉토리에 있는 _MainEventManager_, _BasicNetworkModule_, _POWModule_ 들이다. _BasicNetworkModule_ 은 다른 피어 노드들과 통신하기 위한 API 들을 제공하고 있고, _POWModule_ 은 block을 mining 하기 위한 API 를 제공하고 있다. 또한 이 모듈들이 asynchronous event 를 발생시키기 때문에 이를 다루기 위한 _MainEventManager_ 를 include하였다.


 main 함수안에서 가장 먼저, 아래와 같이 모듈들을 초기화 한다. 
```c++
    MainEventManager mainEventManager;

    BasicNetworkModule basicNetworkModule(gArgs.GetArg("-id", "noid"), &mainEventManager);

    POWModule powModule(&mainEventManager);
```
 이때, 네트워크 상에서 자신의 identification 을 위해, string 변수를 함께 넣어 초기화 하도록 하였다. 자신의 id 값은 커맨드라인 옵션을 통해 전달받거나, 만일 커맨드 라인 옵션이 없을 경우 noid 로 초기화 하도록 한다.
 
 다음으로, 네트워크에서 자신의 이웃 노드들에게 연결을 요청하게 된다.
```c++
    for (auto neighborPeerId : gArgs.GetArgs("-connect"))
        basicNetworkModule.AsyncConnectPeer(PeerId(neighborPeerId));
```
 마찬가지로 커맨드라인 옵션을 통해, 연결하고 싶은 peer node들의 id 를 받아서, 이를 이용해 _BasicNetworkModule_ 의 API 인 AsyncConnectPeer를 통해 연결을 요청하였다. 이처럼, _BasicNetworkModule_ 는 네트워크 연결 및 통신을 위한 몇 가지 API들을 제공하게 된다. 이 API들을 이용해 개발자는 자신의 node를 다른 이웃 노드들과 연결시키거나 연결된 이웃 노드들에게 메시지를 전송하는 등의 작업을 수행할 수 있다. 자세한 API 의 semantic 은 [API 문서](https://github.com/kaistshadow/blockchain-sim/wiki/2.-libBLEEP-API)를 참고하도록 하자.

 마지막으로, mainEventManager가 제공하는 asynchronous한 event들을 처리하기 위한 mainloop 를 구현한다. 
mainEventManager는 네트워크 I/O 등의 asynchronous한 event들을 처리할 수 있으며(ex: 새로운 메시지를 전송 받음), 이를 위해 Wait 라는 API를 제공한다. 유저는 asynchronous event를 받아서 처리하기 위해 아래와 같은 코드를 구현해야한다. Wait는 asynchronous한 event가 발생할 때마다 return을 하게 되며, user는 발생한 asynchronous event를 확인하고 알맞은 로직을 구현할 수 있다. 

```c++
    while(true) {
        mainEventManager.Wait(); // main event loop (wait for next event)
        
        while (mainEventManager.ExistAsyncEvent()) {
            AsyncEvent event = mainEventManager.PopAsyncEvent();
        
            switch (event.GetType()) {
            case AsyncEventEnum::none:

             ...
             ...
            }
        }
    }
```

이제 bitcoin node에 대한 구현을 완료하였다. 비슷한 방식으로 client에 대한 구현도 할 수 있다.

# Executing simple bitcoin

우선 자신의 개발 환경이 ubuntu linux 환경인지를 확인해야한다. 
우리는 BLEEP과 simple bitcoin이 Ubuntu 14.04.05 LTS. 와 Ubuntu 16.04 LTS. 에서 제대로 동작하는 것을 확인하였다. 

#### Prerequisites:
+ Simple bitcoin의 compile 및 install (참고: [Compililing, linking example application](https://github.com/kaistshadow/blockchain-sim/wiki/1.1-Tutorial-:-Blockchain-application-development#compililing-linking-example-application))
+ 블록체인 시뮬레이터(shadow)의 compile 및 install (참고: [Tutorial: Blockchain emulation](https://github.com/kaistshadow/blockchain-sim/wiki/1.2-Tutorial-:-Blockchain-emulation))
+ 블록체인 visualization 서버 설치 (참고: [Install for Blockchain Evaluation](https://github.com/kaistshadow/blockchain-sim/wiki/1.3-Tutorial-:-Blockchain-evaluation#installing-dependencies))


simple bitcoin 의 동작을 web visualization 을 통해 확인해보자.
이를 위해 아래와 같이 `BLEEPeval` 디렉토리로 이동하자.
```bash
cd ../BLEEPeval
```

이제 우리가 제공하는 bitcoin 예제 설정 파일을 이용해 실험 및 event visualization을 동작시키자
```bash
python visualize-events.py config-examples/rc1-pow.xml
```

아래의 주소로 웹브라우저를 통해 접속을 하면 동작 결과를 확인할 수 있다.
ipaddress는 위의 스크립트를 실행한 머신의 ip 주소로 바꾸도록 한다.
```
http://ipaddress:1337/frontend.html
```

아래와 같이 blockchain 이 구성되는 모습을 확인할 수 있다.

[[https://github.com/kaistshadow/blockchain-sim/blob/master/doc/images/visualization-pow.png]]

StartFromBeginning 버튼과 Next 버튼을 이용해, blockchain 이 구성되는 중간중간의 모습을 역시 아래와 같이 확인할 수 있다.

[[https://github.com/kaistshadow/blockchain-sim/blob/master/doc/images/visualization-pow-step.png]]

더 많은 노드에서의 bitcoin 동작을 확인하고 싶다면 아래와 같이 다른 설정 파일을 사용하면 된다. (현재 visualize-events.py는 동시에 하나의 웹서버만 띄울 수 있기 때문에, 다른 설정 파일을 동작시키고 싶다면 Ctrl-C 로 이전 visualize-events.py 스크립트의 동작을 멈춘 후 다시 시작해야한다.)
```bash
python visualize-events.py config-examples/rc1-pow-5.xml
python visualize-events.py config-examples/rc1-pow-large.xml
```
