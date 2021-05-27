이 장에서는 __비트코인__ 을 예로 TPS와 Latency에 대한 측정을 진행한다. 측정 기준은 아래와 같다. 
### 측정 기준
- TPS : (메인 체인의 블록에 추가 되어 있는 트랜잭션 개수)/(블록 사이의 timestamp 차의 합)
- Latency : 트랜잭션이 생성된 순간부터 블록에 올라가기 까지의 시간의 평균

# Plugin Guides
비트코인의 성능 측정을 위한 에뮬레이션 구성은 다음과 같다. 

### Workflow 
- `shadow` 프로세스는 `xml`파일에 정의된 내용을 기반으로 네트워크 및 노드를 구성한다. 
- 비트코인 노드(`libBITCOIND_0.19.1DEV.so`)가 초기화 및 실행된다. 
- 채굴관련 노드(`minerNode_onlymining.so`)가 초기화 및 실행된다. 
  - `minerNode_onlymining.so`는 채굴 노드를 실행 하기 위해 필요한 `plugin`이다. 이 `plugin`이 실행하게 될 rpc request는 다음과 같다.
    ```
      std::string wallet = rpc_request_with_no_params("getnewaddress");    
      rpc_reqeust_with_params("setgeneratetoaddress", params_list);  
    ``` 
- 성능 측정 관련 노드(`libBITCOINTPS_TESTER.so`|`libBITCOINLATENCY_TESTER.so`)가 초기화 및 실행된다. 
  - 트랜잭션 생성 
    - 트랜잭션을 1초에 1000개씩 생성(Latency 측정시 1초에 10개씩 생성)
      ```
              void SetGenerateTimer() {
      #define TX_PER_TICK 1000        // temporary transaction per tick: 1000
      #define TIME_PER_TICK   1       // temporary time per tick: 1 second
                  for (auto &_txGeneratorNode : _txGeneratorNodes) {
                      _txGeneratorNode.SetTxGenerateTimer(TX_PER_TICK, TIME_PER_TICK);
                  }
              }    
      ```
  - 메인체인 모니터링 및 결과 출력
    - 1초에 1번씩 메인체인을 참고하여 결과를(TPS|Latency) 출력.     
      - TPS
        ```
          void setMonitorTimer() {
              _monitorTimer.set<TPSPolicy,&TPSPolicy<NodePrimitives>::_txmonitorcb>(this);
              _monitorTimer.set(1,1);
              _monitorTimer.start();
          }
        ```
      - Latency
        ```
          void setMonitorTimer() {
              _monitorTimer.set<LatencyPolicy,&LatencyPolicy<NodePrimitives>::txmonitorcb>(this);
              _monitorTimer.set(1,1);
              _monitorTimer.start();
          }      
        ```
    - 메인체인의 마지막 블록(`best_tip`)부터 제네시스 블록까지 탐색하여 TPS는 트랜잭션 개수와 타임스탬프를, Latency는 table의 타임스탬프와 해당 트랜잭션이 포함된 블록의 타임스탬프를 기준으로 결과를 출력

### Core Components
- 공통 
  - __libBITCOIND_0.19.1DEV.so__ : `../../external/bitcoin/0.19.1dev/src`에 의해 생성되는 `bitcoind`를 shared object 형식으로 빌드함 
  - __minerNode_onlymining.so__ :  `one_node_setmine.cpp`, `../../testlibs/rpc_client.cpp`로 구성됨, 각 비트코인 노드에게 블록을 채굴하도록 지시하는 노드
- TPS   
  - __libBITCOINTPS_TESTER.so__ : `tps_tester.cpp`와 의존하는 라이브러리로 구성됨
    - 의존하는 라이브러리
      > ~/BLEEPeval/tpstest-library/src/ 폴더에서 생성되는 공유 라이브러리(`BLEEPtps`)<br>
      > ~/BLEEPeval/tpstest-app/bitcoin/ 폴더에서 생성되는 공유 라이브러리(`BitcoinTPSAdapter`)<br>
      > ~/Install/plugins/ 폴더에 생성되는 공유 라이브러리(`libBITCOIND_0.19.1DEV.so`)
- Latency 
  - __libBITCOINLATENCY_TESTER.so__ : `latency_tester.cpp`와 의존하는 라이브러리로 구성됨
    - 의존하는 라이브러리
      > ~/BLEEPeval/tpstest-library/src/ 폴더에서 생성되는 공유 라이브러리(`BLEEPtps`)<br>
      > ~/BLEEPeval/tpstest-app/bitcoin/ 폴더에서 생성되는 공유 라이브러리(`BitcoinTPSAdapter`)<br>
      > ~/Install/plugins/ 폴더에 생성되는 공유 라이브러리(`libBITCOIND_0.19.1DEV.so`)    
# Configuration Guides
- __plugin__ 정의 및 사용법 (e.g., `~/BLEEPeval/tpstest-app/bitcoin/test-BitcoinTPS.xml`)
  ```
  <!-- 비트코인 plugin 정의 -->
    <plugin id="bitcoind" path="libBITCOIND_0.19.1DEV.so"/>
  <!-- 채굴 관련 plugin 정의 -->  
    <plugin id="client" path="minerNode_onlymining.so"/>
  <!-- 트랜잭션 생성 및 TPS 측정 plugin 정의 -->  
    <plugin id="BITCOINTPS_TESTER" path="libBITCOINTPS_TESTER.so"/>

  <!-- 비트코인 노드 정의 --> 
    <node id="bcdnode0" iphint="1.0.0.1">
      <application arguments="-debug -reindex -datadir=data/bcdnode0 -port=18333 -txindex=1 -fallbackfee=0.0002 -rpcuser=a -rpcpassword=1234 -rpcport=11111 -rpcallowip=1.0.0.1/0 -rpcbind=1.0.0.1 -addnode=1.1.0.1:18333 -addnode=1.1.0.1:18333 -algorithm=pow -difficulty=3 -limitancestorcount=200000 -limitdescendantcount=200000 -limitdescendantsize=20000 -maxmempool=1000" plugin="bitcoind" time="0"/>
    </node>
    <node id="bcdnode1" iphint="1.1.0.1">
      <application arguments="-debug -reindex -datadir=data/bcdnode1 -port=18333 -txindex=1 -fallbackfee=0.0002 -rpcuser=a -rpcpassword=1234 -rpcport=11111 -rpcallowip=1.1.0.1/0 -rpcbind=1.1.0.1 -addnode=1.0.0.1:18333 -addnode=1.0.0.1:18333 -algorithm=pow -difficulty=3 -limitancestorcount=200000 -limitdescendantcount=200000 -limitdescendantsize=20000 -maxmempool=1000" plugin="bitcoind" time="0"/>
    </node>
  <!-- 채굴관련 노드 정의 -->  
    <node id="client0">
      <application arguments="1.0.0.1:11111 293 " plugin="client" time="3"/>
    </node>
  <!-- 트랜잭션 생성 및 TPS 측정 노드 정의 -->    
    <node id="txgenerator" iphint="99.99.0.1">
      <application plugin="BITCOINTPS_TESTER" time="5" arguments="99.99.0.1 99.99.0.2 2"/>
    </node>
  ```
# Script tool Guides 
- 스크립트 도구 사용 방법 <br>
BLEEP에서 제공하는 스크립트 도구인 `BLEEPeval/tpstest-app/start_test.sh`을 이용해 비트코인 노드의 성능측정(TPS/Latency)이 가능하다. `start_test.sh`는 2개의 파라미터(난이도, 정책)를 입력받으며, 난이도는 1부터 3까지 입력 가능하며 숫자가 낮을 수록 난이도가 높고 정책은 TPS 및 Latency를 입력 가능하다. 실행방법은 아래와 같다. 
  ```
  start_test.sh [Difficulty:1/2/3] [Policy:TPS/Latency]
  ```

- The logs
	- Console output
    ```
    cleanup data start!
    Datadir Bootstrapping success!
    ** Starting Shadow v1.12.1-268-gb5ec6029 2021-04-13 (built 2021-05-10) with GLib v2.56.4 and IGraph v0.7.1
    ** Stopping Shadow, returning code 0 (success)   
    ```
	- shadow output <br>
    시뮬레이션이 성공적으로 끝나면 shadow.data 디렉토리가 생성될 것이며, shadow.data 디렉토리 구조는 다음과 같음.이 중 output.txt는 shadow console 로그를 담고있는 파일임. 이 파일을 통해 xml에 정의된 플러그인이 제대로 시뮬레이션이 되었나 확인을 할 수 있으며, 시뮬레이션을 전체적으로 검증하는데 유용함.
    ```
    shadow.data
    └──  hosts
        ├── bcdnode0
        ├── bcdnode1        
        ├── client0        
        └── txgenerator            
    ```
    - hosts 폴더  
      - `bcdnode*` 폴더 <br>
      비트코인 플러그인의 실행 로그. `bitcoind`를 실행 했을때 터미널 콘솔로 print되는 로그. 이 로그를 통해서 해당 비트코인 플러그인의 동작을 검증을 할 수 있음. 
      - `client*` 폴더 <br>    
      비트코인 플러그인의 rpc function request 플러그인임. 플러그인에는 "getnewadress", "setgeneratetoaddress" 이렇게 2개의 rpc function request가 정의가된 플러그인임. 이 플러그인은 rpc response를 받은 결과가 print되어 이렇게 로그로 결과가 기록이됨.  
        ``` 
        cat stdout-client0.client.1000.log
        start client 
        request : {"id":"rpc_client","jsonrpc":"1.0","method":"getnewaddress","params":[]}
        -- result : {"result":"3KkmX1aKBp6mqidxEm5TLwdGh6cdfA13oP","error":null,"id":"rpc_client"}

        wallet:3KkmX1aKBp6mqidxEm5TLwdGh6cdfA13oP
        request : {"id":"rpc_client","jsonrpc":"1.0","method":"setgeneratetoaddress","params":["3KkmX1aKBp6mqidxEm5TLwdGh6cdfA13oP"]}
        -- result : {"result":true,"error":null,"id":"rpc_client"}
        ```
      - txgenerator 폴더 <br>
      `tps_tester.cpp`에 정의된 트랜잭션을 생성하는 노드와 TPS를 모니터링하는 노드의 실행로그. 아래와 같이 기본 정보 및 TPS 측정결과가 기록이됨 
        ```
        tps_tester start 99.99.0.1, 99.99.0.2
        exported bitcoin main start!
        addTarget : 1.0.0.1 18333
        addTarget : 1.1.0.1 18333
        generator node objects are constructed 1
        Socket is successfully connected
        OpAfterConnect for node [99.99.0.2]
        Socket is successfully connected
        OpAfterConnect for node [99.99.0.2]
        Socket is successfully connected
        OpAfterConnect for node [99.99.0.1]
        Socket is successfully connected
        OpAfterConnect for node [99.99.0.1]
        ...
        [INV] MSGBLOCK: hash = 000007bfed94ba1fdf11d37e52882e6cd4bf31b8e3431a6c6fc1ba76a9e029d0 txcnt = 18 from = 20
        [INV] MSGBLOCK: hash = 000007bfed94ba1fdf11d37e52882e6cd4bf31b8e3431a6c6fc1ba76a9e029d0 txcnt = 18 from = 19
        TPS = 16.5143 / txcount :4046
        [INV] MSGBLOCK: hash = 00000e3c4776325396d66d7fe93ebc1e6ddef910d21f3a5823cc5c923d40cafe txcnt = 54 from = 20
        [INV] MSGBLOCK: hash = 00000e3c4776325396d66d7fe93ebc1e6ddef910d21f3a5823cc5c923d40cafe txcnt = 54 from = 19
        TPS = 16.6667 / txcount :4100
        [INV] MSGBLOCK: hash = 000009b97a548793dfbb2f063dd16cf8349f1eff6ae9ed43caf01c400525e7a8 txcnt = 456 from = 20
        TPS = 17 / txcount :4556       
        ```
      

# Example experiment 

### Run
```
cd BLEEPeval/tpstest-app/bitcoin
sh start_test.sh 3 TPS
```
> ### Simulation condition
> Node : 2 <br>
> Network topology : linear (default)<br>
> Simulation time : 300 sec <br>
> Block difficulty : 3 <br>
> Policy : TPS <br>

### output
- console output
  ```
  cleanup data start!
  Datadir Bootstrapping success!
  ** Starting Shadow v1.12.1-268-gb5ec6029 2021-04-13 (built 2021-05-10) with GLib v2.56.4 and IGraph v0.7.1
  ** Stopping Shadow, returning code 0 (success) 
  ```
- shadow output
  ```
  └── hosts
      ├── bcdnode0
      │   └── stdout-bcdnode0.bitcoind.1000.log
      ├── bcdnode1
      │   └── stdout-bcdnode1.bitcoind.1000.log
      ├── client0
      │   └── stdout-client0.client.1000.log
      └── txgenerator
          └── stdout-txgenerator.BITCOINTPS_TESTER.1000.log
  ```