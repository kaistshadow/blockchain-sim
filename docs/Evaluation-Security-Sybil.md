BLEEP은 P2P 모듈 취약점을 분석하기 위한 테스트 프레임워크 및 API를 제공한다. 
제공하는 테스트 프레임워크를 통해 블록체인 구현에 상관없이 공통적으로 적용 가능한 P2P 보안 테스트를 구동시킬 수 있고, 취약점을 분석할 수 있다. 

# P2P Security Test Overview
BLEEP은 [Emulation](https://github.com/kaistshadow/blockchain-sim/blob/feature/191/documentation/docs/Emulation-Guide.md) 기능을 통해, 블록체인 네트워크를 구성 및 동작시키고 그 위에서 P2P 프로토콜이 정상적으로 동작하는지를 검증한다.
BLEEP은 임의의 블록체인 바이너리(혹은 P2P 구현 모듈)에 대한 테스트를 위해, 공통적으로 사용할 수 있는 에뮬레이터, sybil 라이브러리, 어댑터 API 등을 제공한다.

BLEEP P2P security 테스트의 구성 요소는 아래와 같다. 
- [Emulator](https://github.com/kaistshadow/blockchain-sim/blob/feature/191/documentation/docs/Emulation-Guide.md) : 실제 네트워크를 띄울 필요 없이 다수의 노드로 구성된 P2P 네트워크를 에뮬레이션
- [Sybil Library](Evaluation-Security-Sybil.md#sybil-library) : 공격을 위한 P2P 환경을 구성하고, 공격에 필요한 sybil 노드들을 구성하여 타겟 노드에 대한 공격 실행 및 P2P 취약점을 검증
- [Adapter](Evaluation-Security-Sybil.md#adapter) : Sybil 라이브러리를 임의의 타겟 노드에 대해 동작시키기 위한 어댑터 
- [Tester](Evaluation-Security-Sybil.md#tester) : Sybil 라이브러리와 Adapter를 이용해 실제 테스트가 구현된 프로그램 


# Sybil Library
P2P Security Test를 위해선 대규모의 Sybil 노드들을 에뮬레이션 상에서 동작시키고, 분석하고자 하는 타겟 블록체인(혹은 P2P 모듈)과 연결 및 메시지 통신이 가능해야 한다.
BLEEP은 이러한 작업들을 쉽게 할 수 있도록 sybil library를 제공한다. 이 라이브러리를 이용해 쉽게 P2P Security Test를 위한 [Tester](Evaluation-Security-Sybil.md#tester) 개발이 가능하다.
Sybil library의 소스는 BLEEPeval 폴더에 있다. ([링크](https://github.com/kaistshadow/blockchain-sim/tree/master/BLEEPeval/sybiltest-library/src))

Sybil library가 제공하는 주요 헤더 및 클래스는 아래와 같다.
- [AttackBox](https://github.com/kaistshadow/blockchain-sim/blob/master/BLEEPeval/sybiltest-library/src/AttackBox.h) : 공격하고자하는 타겟 노드를 설정하고, 공격을 설정하고 시작하는 API 제공. 테스트하고자 하는 보안 공격의 종류에 따라 Policy 클래스를 설정할 수 있으며, 테스트하고자 하는 타겟 블록체인에 따라 IP database 및 Adapter를 설정할 수 있다. 테스트를 위한 파라미터도 설정한다.
- [Policy](https://github.com/kaistshadow/blockchain-sim/tree/master/BLEEPeval/sybiltest-library/src/policy) : 테스트하려는 공격이 구현되어 있는 클래스. 여러 종류의 Node들로 네트워크를 구성하고, target node에 대한 공격이 이루어졌는지를 확인한다. 현재 두 개의 policy(incoming connection 에 대한 eclipse 공격, EREBUS 공격)를 제공한다.
- [IP database](https://github.com/kaistshadow/blockchain-sim/tree/master/BLEEPeval/sybiltest-library/src/ipdb) : 공격을 위해 필요한 노드들의 IP를 관리하는 클래스. 공격에 따라 다른 종류의 IP database를 활용할 수 있도록 여러 클래스가 구현되어 있다. 공통적으로, 네트워크 상에 존재하는 Benign 노드의 IP(ReachableIP), 네트워크 상에 존재하지 않는 노드의 IP(UnreachableIP), 공격을 위한 sybil node의 IP(shadowIP), sybil node 중에 active하게 target에 연결을 시도하는 IP(attackerIP)들을 저장하고 관리한다. 각 IP들의 uptime(네트워크 상에 online으로 있는 시간)도 저장 및 관리한다.
- [Node](https://github.com/kaistshadow/blockchain-sim/tree/master/BLEEPeval/sybiltest-library/src/node) : 공격에 필요한 구성 노드들을 구현하는 클래스. sybil 노드(ShadowNode), sybil 노드 중에 target에 직접 연결을 시도하는 노드(ShadowActiveNode), target으로부터의 연결을 기다리는 정상 노드(BenignNode)로 나뉘어져 구현되어 있다. 또한 모든 노드들은 NodePrimitives를 상속받는다. 이 NodePrimitives는 타겟 노드에 따라 다르게 구현될 수 있는 P2P 메세지 프로토콜을 Sybil Library가 상호작용할 수 있도록 한다. NodePrimitives는 아래에 설명할 Adapter에 해당하는 부분으로 별도의 클래스 및 API로 분리되어 구현된다.


# Adapter
Adapter는 타겟 노드에 따라 다르게 구현될 수 있는 P2P 메세지 프로토콜을 Sybil Library가 문제없이 인식하고 P2P 공격 테스트를 진행할 수 있도록 하기 위해 필수적이다.
즉 사용자는 Sybil Library를 사용할 때, 테스트하고자하는 타겟 노드들에 따라 별도로 Adapter를 구현해야한다.

이 Adapter는 코드 상으로는 Sybil Library의 AttackBox가 Template Parameter로 받게되는 NodePrimitives 템플릿의 구현체를 의미한다. 
- NodePrimitives 템플릿은 Node class의 부모 클래스로서, 모든 Node들이 상속받게되는 최상단의 class이다. 
- 편의를 위해 현재의 NodePrimitives는 Node 자신의 IP를 의미하는 `_myIP`, Node의 타입을 의미하는 `_type`, TCP메시지를 버퍼링하는 `_mTCPControl`, 테스트 진행 상황에 대한 stat을 저장하는 AttackStat 클래스에 대한 포인터(`_attackStat`), 테스트 시 사용한 IP Database 클래스에 대한 포인터(`_ipdb`) 등을 멤버변수로 가지고 있다. 
- 또한 Adapter가 메시지를 쉽게 전송하게끔 도와주는 `SendMsg` 함수를 멤버 함수로 가지고 있다. 이 멤버변수들과 함수들을 아래에 설명하는 Adapter Callback API를 구현할 때에 사용가능하다.
- 위의 부분들은 새로운 Adapter를 구현할 때 그대로 복사하여 사용하면 된다. 
- 참고 : 추후에 리팩토링을 통해 아래에 설명할 Adapter Callback 로직과 위의 utility 관련 변수 및 함수들을 분리할 예정이다.

Adapter(즉, NodePrimitives 구현체)는 테스트하고자 하는 Policy에 따라 다른 종류의 callback 함수들을 새롭게 구현해야한다.

- EREBUS attack policy
  - OpAfterConnect : Sybil Library의 Node가 타겟에 연결된 직후에 불리는 함수. Node가 직접 연결 시도를 한 경우에만 해당. version exchange와 같은 protocol init 과정을 시작해야함
  - OpAfterRecv : Sybil Library의 Node가 타겟으로부터 메시지를 받았을 때 불리는 함수. 메시지를 파싱하고 메시지에 따라 새로운 프로토콜 메시지를 만들어 reply해야함.
  - OpAddrInjectionTimeout : Sybil Library의 ShadowActiveNode가 타겟에 Addr 메시지를 전송하기 위한 목적으로 일정 주기마다 불리는 함수. Addr메시지를 생성하여 타겟에 전송해야함.
- Incoming Eclipse attack policy
  - OpAfterConnect : Sybil Library의 Node가 타겟에 연결된 직후에 불리는 함수. Node가 직접 연결 시도를 한 경우에만 해당. version exchange와 같은 protocol init 과정을 시작해야함
  - OpAfterRecv : Sybil Library의 Node가 타겟으로부터 메시지를 받았을 때 불리는 함수. 메시지를 파싱하고 메시지에 따라 새로운 프로토콜 메시지를 만들어 reply해야함.
  - OpAfterDisconnect : Sybil Library의 Node가 타겟으로부터 연결이 끊겼을 때 불리는 함수. 다시 타겟에 대한 새로운 연결을 시도해야함.

현재 EREBUS attack policy를 bitcoin v0.19.1에 대해 적용하기 위한 [Adapter](https://github.com/kaistshadow/blockchain-sim/blob/master/BLEEPeval/sybiltest-app/bitcoin/BitcoinNodePrimitives.h)와 
Eclipse attack policy를 BLEEP P2P module에 적용하기 위한 [Adapter](https://github.com/kaistshadow/blockchain-sim/blob/1b4d617ea1dff82f4c3c99c418394c0af5ed029d/BLEEPeval/sybiltest-app/BLEEPP2P/BLNodePrimitives.cpp)가 구현되어 있다.

또한, 각각의 Adapter들은 메시지 파싱 및 생성을 보다 쉽게 하기 위해 타겟 노드의 header(예:비트코인 v0.19.1의 `<netmessagemaker.h>`)를 이용하는 형태로 구현되어 있다. 


# Tester
테스터는 Sybil library와 Adapter를 이용해 실제 P2P 테스트를 구현하는 프로그램이다. 
Sybil library API 및 Adapter를 사용해 P2P 공격 테스트를 간단히 구현하게 된다. 

```
#include <bitcoind.h>
#include <AttackBox.h>                      // Sybil Library API
#include <policy/ErebusAttackPolicy.h>      // Sybil Library API
#include <ipdb/BitcoinIPDatabase.h>         // Sybil Library API

#include "BitcoinNodePrimitives.h"          // Adapter for Bitcoin
#include "BitcoinNodeParams.h"              

using namespace sybiltest;

int main() {
    exported_main(); // initialize bitcoin

    AttackBox<ErebusAttackPolicy, BitcoinNodePrimitives, BitcoinNodeParams, BitcoinIPDatabase> attackBox;   // AttackBox 생성
    attackBox.setTarget("1.0.0.1", 8333);                                                                   // 타겟 노드 설정
    if (!attackBox.setupAttack()) {                                                                         // 공격 설정
        std::cout << "setup attack failed" << "\n";
        return -1;
    }
    attackBox.startAttack();                                                                                // 공격 수행
    std::cout << "attack finished" << "\n";                                                                 // 공격 테스트 완료 후 종료

    return 0;                                                                                               
}
```


# P2P Security Test Example

- 테스트 하고자 하는 타겟노드를 BLEEP에서 에뮬레이션 할 수 있는 플러그인 형태(즉, shared object)로 컴파일한다. 

- Sybil Library의 AttackBox를 이용하는 [Tester](Evaluation-Security-Sybil.md#tester)를 구현한다.
```
#include <AttackBox.h>

int main() {
    AttackBox< , , , > attackBox;
    attackBox.setTarget("1.0.0.1", 8333);                                  // 타겟 노드 설정
    if (!attackBox.setupAttack()) {                                        // P2P 공격을 위한 sybil network 설정
        std::cout << "setup attack failed" << "\n";
        return -1;
    }
    attackBox.startAttack();                                               // P2P 공격 수행
    std::cout << "attack finished" << "\n";                                // 공격 테스트 완료 후 종료

    return 0;      
    
}
```

- Tester에서 테스트하고자 하는 공격을 설정한다. (현재 Sybil library가 제공하는 policy는 `ErebusAttackPolicy`와 `EclipseIncomingAttackPolicy`가 있다.)
```
#include <policy/ErebusAttackPolicy.h>

...
int main() {
    AttackBox<ErebusAttackPolicy, , , > attackBox;
...
```

- Tester에서 공격을 위해 사용하는 IP Database를 세팅한다. (현재 Sybil Library가 제공하는 IP database는 `BitcoinIPDatabase`와 `SimpleIPDatabase`가 있다.)
```
#include <ipdb/BitcoinIPDatabase.h>

...
int main() {
    AttackBox<ErebusAttackPolicy, , ,BitcoinIPDatabase> attackBox;
...
```

- Adapter를 구현하여, Tester에서 include한다. 
``` 
==== BitcoinNodePrimitives.h ====

class BitcoinNodePrimitives {
    private:
    ... // 각종 utility
    
    public:
        void OpAfterConnect(int conn_fd);

        void OpAfterConnected(int data_fd); 

        void OpAfterRecv(int data_fd, std::string recv_str);

        void OpAfterDisconnect();

        void OpAddrInjectionTimeout(std::chrono::system_clock::duration preparePhaseDuration, int periodLength,
                                    double ipPerSec, double shadowRate);
}     

==== BitcoinNodePrimitives.cpp ====

void BitcoinNodePrimitives::OpAfterConnect(int conn_fd) {
    // 관련 로직 구현
}

void BitcoinNodePrimitives::OpAfterRecv(int data_fd, string recv_str) {
    // 관련 로직 구현
}   

void BitcoinNodePrimitives::OpAddrInjectionTimeout(std::chrono::system_clock::duration preparePhaseDuration,
                                                   int periodLength, double ipPerSec, double shadowRate) {
    // 관련 로직 구현
}    

void BitcoinNodePrimitives::OpAfterConnected(int data_fd) {
    // do nothing
}
void BitcoinNodePrimitives::OpAfterDisconnect() {
    // do nothing
}

==== tester.cpp ====
#include "BitcoinNodePrimitives.h"

...
int main() {
    AttackBox<ErebusAttackPolicy, BitcoinNodePrimitives, ,BitcoinIPDatabase> attackBox;
...
```

- 실험 관련 파라미터를 정의하여 Tester에서 사용한다.
```
==== BitcoinNodeParams.h ====
    struct BitcoinNodeParams {
        //============================== parameters related to EREBUS attack test ======================================
        static constexpr int targetPort = 8333;
        static constexpr int targetOutgoingConnNum = 5;
        static constexpr int reachableIPNum = 100000;
        static constexpr int unreachableIPNum = 1000000;
        static constexpr int shadowIPNum = 200000;
        static constexpr int addrInjectionStartTime = 15;
        static constexpr int addrInjectionDelay = 900;
        static constexpr double addrInjectionIPPerSec = 2;
        static constexpr double addrInjectionShadowRate = 0.9;
        static constexpr std::chrono::system_clock::duration preparePhaseTimeLength = std::chrono::hours(30*24);
    };
    
==== tester.cpp ====
#include "BitcoinNodeParams.h"

...
int main() {
    AttackBox<ErebusAttackPolicy, BitcoinNodePrimitives, BitcoinNodeParams, BitcoinIPDatabase> attackBox;
...
```

- Tester를 plugin형태(shared object)로 컴파일한 뒤, 타겟 노드와 함께 emulation을 동작시키기 위한 실험 설정을 제작한다.
```
==== test-BitcoinP2P.xml ====
...
  <!-- the plug-ins we will be using -->
  <plugin id="bitcoind" path="libBITCOIND_0.19.1DEV.so"/>
  <plugin id="BITCOINP2P_TESTER" path="libBITCOINP2P_TESTER.so"/>

  <kill time="17776000"/>
  <node id="bcdnode0" iphint="1.0.0.1">
    <application
            arguments="-debug -datadir=data/bcdnode0 -port=8333 -rpcuser=a -rpcpassword=1234 -rpcport=11111 -rpcallowip=1.0.0.1/0 -rpcbind=1.0.0.1 -erebustest"
            plugin="bitcoind" time="0"/>
  </node>


  <node id="tester" iphint="99.99.0.1">
    <application plugin="BITCOINP2P_TESTER" time="5" arguments=""/>
  </node>
...
```

- BLEEP 내의 shadow를 이용해 실험을 동작시킨다.
```
$ ./path/to/shadow test-BitcoinP2P.xml
```

- 테스트 결과는 아래의 폴더에 log형태로 저장된다.
```
$ less ./shadow.data/hosts/tester/*
$ less ./shadow.data/hosts/bcdnode0/*
```

위 예제는 Bitcoin v0.19.1에 대한 EREBUS 공격 테스트를 제작한 것으로 아래의 위치에 구현되어 있다. <br>
https://github.com/kaistshadow/blockchain-sim/tree/master/BLEEPeval/sybiltest-app/bitcoin
