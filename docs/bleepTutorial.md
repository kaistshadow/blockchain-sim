# 0. Install 
```
git clone https://github.com/kaistshadow/blockchain-sim.git 
cd blockchain-sim; python setup.py --bitcoin
```
# 1. Ready for simulating blockchain
## 1-1. shadow
 shadow는 네트워크 동작을 시뮬레이션을 하기 위한 오픈소스 프로그램임. shadow에서 대해는 [튜토리얼](https://github.com/shadow/shadow/blob/main/docs/2-Getting-Started-Tutorial.md) 을 보면 자세히 알 수 있다. shadow는 BLEEP install 과정에서 설치가 되기에 별도로 설치를 할 필요가 없다.
## 1-2. Plug-in
 shadow에서 네트워크에 참여하는 시뮬레이션된 노드를 plugin이라고 말함. 여기서 plugin은 바이너리 파일 형식이 아닌 shared object 형식이다. 예를들어 BLEEP에서 비트코인을 시뮬레이션할 때 사용되는 플러그인은 bitcoin의 실행파일인 bitcoind 바이너리 파일이 아닌, shared object형식으로 변환된 bitcoind.so 파일이다. shadow 프로그램은 loader 레벨 가상화를 통해 시뮬레이션을 진행하기 때문에, 시뮬레이션할 프로그램이 shared object 형식이어야 한다. 즉  [BLEEP](https://github.com/kaistshadow/blockchain-sim/wiki/1.-BLEEP-Introduction)은 C/C++기반의 블록체인의 바이너리 파일을 shared object 형식으로 포팅을 하여, 시뮬레이션함.

## 1-3. XML
XML파일에서 시뮬레이션할 네트워크 토폴로지와 플러그인을 정의한다. 정의된 xml파일은 shadow실행 시 argument파일로 입력이 되어 다음과 같이 실행이 된다. 이렇게 시뮬레이션이 실행이 되면 xml에서 설정한 값에 따라 네트워크가 시뮬레이션이 된다. 
```
shadow example.xml
```
 네트워크 토폴로지 설정에 대해 자세히 알고 싶으면 [shadow-network config](https://github.com/shadow/shadow/blob/main/docs/3.2-Network-Config.md) 참고. 

<br>

# 2. Plug-in 종류
```
├── plugins
│   ├── libBITCOIND_0.19.1DEV.so (Bitcoin plugin)
│   ├── libBITCOINTPS_TESTER.so (TxGenerator plugin)
│   ├── libBITCOIN_MONITOR.so (Monitor node plugin)
│   ├── minerNode_rpc.so (Mining rpc plugin)
│   └── rpc.so (customed rpc plugin)
```
## 2-1. Blockchain plugin (libBITCOIND_0.19.1DEV.so)
블록체인은 비트코인 이외에 무수히 많이 존재한다.(Monero, Litecoin, Ripple, EOS 등). 현재 BLEEP에서는 C++/C 기반의 블록체인에 대해서만 시뮬레이션이 가능하다. 하지만 그 외에 언어로 개발된 블록체인 또한 점차 시뮬레이션을 지원할 것이다. 이러한 모든 블록체인들이 BLEEP에서 시뮬레이션할 plugin 중에 하나이다. 이러한 plugin은 xml에 정의가 되어서, 시뮬레이션될 node 정의 과정에서 사용된다. 해당 플러그인은 bitcoin의 플러그인이다. 플러그인 파일은 libBITCOIND_0.19.1DEV.so이며, 플러그인 정의와 사용법은 다음과 같다.  <br>
#### 플러그인 정의
```
  <plugin id="bitcoind" path="libBITCOIND_0.19.1DEV.so"/>
```
#### 플러그인 사용
```
  <node id="bcdnode0" iphint="1.0.0.1">
    <application arguments="" plugin="bitcoind" time="0"/>
  </node>
```
## 2-2. TxGenerator plugin (libBITCOINTPS_TESTER.so)
블록체인을 시뮬레이션을 하여, TPS를 측정하기 위해서는 Transaction을 시뮬레이션 환경에서 계속해서 생성이 되고, 전파가 되어야 한다. 이를 rpc client로 Transaciton을 꾸준하게 생성하는 과정은 rpc request의 오버헤드가 있기에 비효율적이다. 그래서 별도의 Transation generator를 생성하여, 좀 더 효율적으로 TPS를 측정할 수 있게 했다.
#### 플러그인 정의

```
  <plugin id="BITCOINTPS_TESTER" path="libBITCOINTPS_TESTER.so"/>
```

#### 플러그인 사용
argv[1] : IP address  <br>
argv[2] : Miner node count
```
  <node id="txgenerator" iphint="99.99.0.1">
    <application plugin="BITCOINTPS_TESTER" time="5" arguments="99.99.0.1 2"/>
  </node>
```

## 2-3. Monitor node plugin (libBITCOIN_MONITOR.so)
emulation의 결과를 효율적으로 evalution(TPS, latency측정 등) 하기 위해서는 블록체인에서 각 노드의 블록, Tx 동기화 상태를 알고 있어야 한다. 이를 효율적으로 관리를 해줄 Monitor Node를 별도로 생성을 하여, Monitor Node를 통해 emulation된 결과들은 효율적으로 evalution할 수 있다. 
#### 플러그인 정의

```
  <plugin id="BITCOIN_MONITOR" path="libBITCOIN_MONITOR.so"/>
```

#### 플러그인 사용
argv[1] : IP address  <br>
argv[2] : Miner node count
```
  <node id="monitor" iphint="99.99.0.2">
    <application plugin="BITCOIN_MONITOR" time="5" arguments="99.99.0.2 2"/>
  </node>
```


## 2-4. Mining rpc plugin (minerNode_rpc.so)
마이닝 노드를 실행을 하기 위해 필요한 플러그인이다. 이 플러그인이 실행하게 될 rpc request는 다음과 같다. (밑에 코드는 minerNode_rpc.cpp 소스코드의 일부분임)
```
  std::string wallet = rpc_request_with_no_params("getnewaddress");
  rpc_reqeust_with_params("validateaddress", params_list);
  rpc_reqeust_with_params("setgeneratetoaddress", params_list);
  rpc_request_no_return_no_params("getmempoolinfo");
```
먼저 getnewaddress rpc request를 통해 bitcoin wallet을 생성을 하고, 생성된 wallet에 대한 검증을 validateaddress rpc request를 통해 진행한다. 검증이 완료되면 해당 wallet 주소를 이용하여 setgeneratetoaddress (mining request) rpc를 실행하여, 마이닝이 활성화 된다. 즉 Miner node rpc client 플러그인은 마이닝을 위해 필요한 rpc 함수를 모아 놓은 플러그인이다. 플러그인은 minerNode_rpc.so 이며, 다음과 같이 정의를 하여 사용을 한다.

#### 플러그인 정의
```
  <plugin id="MinerRPC" path="minerNode_rpc.so"/>
```
#### 플러그인 사용
```
  <node id="MinerRPC_" iphint="1.0.0.1">
    <application arguments="" plugin="MinerRPC" time="0"/>
  </node>
```

## 2-5. customed rpc plugin (rpc.so) 
시뮬레이션 과정에서는 블록체인을 핸들링 할 수 없다. 그렇기에 시뮬레이션 전에 핸들링에 대해 미리 정의를 해주어야한다. 예를들어 Bitcoind를 시뮬레이션할 때, "getblockchaininfo" rpc response를 하고 싶은데, 시뮬레이션 과정에서는 외부 핸들링이 불가능하기에 핸들링 해주고 싶은 rpc function들을 미리 정의를 하여 플러그인을 만들어줌.
```
    params = Json::arrayValue;
    bitcoin_rpc_request("getnewaddress", params);
    std::string wallet = json_resp["result"].asString();
    std::cout<<"wallet:";
    std::cout<<wallet;
    std::cout<<"\n";
    params.clear();
    params = Json::arrayValue;
    rpc_validateaddress(wallet, argv[1]);
    rpc_generatetoaddress(wallet, argv[1]);
```
다음 코드는 bitcoind에게 'getnewaddress', 'validateaddress', 'generatetoaddress'의 rpc request들의 정의된 소스코드임. 해당 소스코드를 shared object 형식으로 변화하여 플러그인을 만들고, 만들어진 plugin을 xml에 정의를 하여 사용을 하면 시뮬레이션 과정에서 bitcoind rpc 핸들링이 가능함. 플러그인은 rpc.so이며, 이는 커스텀하여 개발을 하는 플러그인이라 플러그인 이름은 개발자들이 정하기 나름이다.
#### 플러그인 정의
```
  <plugin id="rpc" path="rpc.so"/>
```
#### 플러그인 사용
argv[1] : IP address <br>
argv[2] : Miner node count
```
  <node id="client0">
    <application arguments="1.0.0.1:11111 3 " plugin="rpc" time="5"/>
  </node>
```

다음 명령어를 통해 시뮬레이션이 진행되며, 전반적인 과정을 밑에 그림으로 표현함.
```
shadow example.xml
```
xml 파일에는 시뮬레이션할 실행파일의 plugin을 정의를 해준다. 밑에 example.xml을 보면 bitcoind를 plugin 셋팅을 해주어 사용을 하였다. 위에서 말했듯이 시뮬레이션 환경에서 개발자가 직접적으로 bitcoind를 컨트롤 할 수 없기에 사전에 컨트롤이 필요한 부분을 미리 정의를 하여, 이 또한 플러그인으로 셋팅을 하여 사용을 한다. 이렇게 정의된 xml파일을 shadow로 실행을 하면 밑에 그림 처럼 시뮬레이션이 작동하고, 종료되면 결과 파일로 plugin의 로그들 (bitcoind.log, client.log)과 shadow 로그 (output.txt)가 리턴된다.
![](https://postfiles.pstatic.net/MjAyMTAzMjRfMTgw/MDAxNjE2NTYxNjI0OTg5.K1LIOiir9TqhetT2d9t_pXi9z2nHSE5N1Qe1GUC8nGcg.JfOevJ47LcKwHLfKm9pxy9fzD66pptEszaqvMrAtpnIg.PNG.tkdlqm2/image.png?type=w966)


# 3. xml에서 plugin 사용법

## 3-1. plugin arguments 정의
bitcoind 실행 플래그 옵션은 [다음](https://gist.github.com/frz-dev/5390b954f9cfe3c8daa757c6c8da92ef)과 같음. 이는 bitcoind를 실행하기 위해 필요한 설정 또는 옵션 값을 정의를 해주는 것임. 예를들어 비트코인을 시뮬레이션할 때 testnet으로 시뮬레이션을 하고싶은 경우, shadow xml에 정의하는 방법대로 하면됨.

#### normal
```
./bitcoind --testnet
```

#### shadow
```
  <node id="bcdnode0" iphint="1.0.0.1">
    <application arguments="--testnet" plugin="bitcoind" time="0"/>
  </node>
```
xml파일 에서 시뮬레이션할 node에 대한 정의를 하는 과정에 application arguments에서 bitcoind에서 지원하는 플래그 값들을 정의해주면 됨. <br>

## 3-2. IP address 셋팅
node 정의 과정에서 다음과 같이 iphint로 셋팅을 해줌. 이렇게 셋팅된 IP는 시뮬레이션 상에서 해당 노드에게 정의된 IP임.
```
  <node id="bcdnode0" iphint="1.0.0.1">
```
<br>

## 3-3 Simulation time 설정
전체 simulation time 설정은 다음과 같이 하면 됨. 이는 시뮬레이션이 동작하는 시간을 의미함.
```
  <kill time="10"/>
```
각 노드마다 시뮬레이션 시간이 시작되는 시간의 설정은 다음과 같음. time="5" 라고 정의를 한 것은 시뮬레이션 시간 5초에 동작함을 의미함.
```
    <application arguments="" plugin="client" time="5"/>
```

# 4. How to start
다음 명령은 애뮬레이션을 실행하는 스크립트 명령어임. 실행이 되면 xml파일을 생성하기 위한 값을 콘솔로 요구함. 알맞게 값을 입력하면, xml파일이 생성되면서 시뮬레이션이 진행됨.
```
cd BLEEPemul/emulation; python3 start_emulation.py
>
4-1 Input node count (only integer more than 1) : 
4-2 Input simulation time (sec) : 
4-3 Input mining algorithm(pow/coinflip) : 
4-4 Input difficulty(1/2/3) : 
4-5 Input transaction injector (enable/disable) :       
4-6 input number of transcations ( -1 : infinite number ): 
4-7 input number of Bitcoins transferred (minimum amount : 0.0000546) : 
4-8 Input transaction interval (sec) : 

```

#### 4-1 Node count
시뮬레이션할 노드의 개수를 입력하는 콘솔임. 무조건 자연수만 입력을 받음. <br><br>

#### 4-2 Simulation time
시뮬레이션할 시간을 입력받는 콘솔임. 무조건 자연수만 입력을 받음. <br><br>

#### 4-3 Mining algorithm
시뮬레이션될 비트코인의 Mining algorithm을 선택하는 콘솔임. 기존의 알고리즘인 pow와, pow 블록 생성 로직을 추상화하여 복잡한 연산을 생략시키고, 포화성 분포로 블록 생성 알고리즘을 대체한 알고리즘인 coinflip 중에 입력을 받음. <br><br>

#### 4-4 Network Difficulty
시뮬레이션될 비트코인의 블록생성 시간에 직접적인 영향을 주는 난이도의 단계를 선택하는 알고리즘임. 총 3단계로서 난이도와 단계는 반비례관계임. 즉 1단계 난이도가 가장 어려우며, 3단계 난이도가 가장 쉬운 난이도임. <br><br>

#### 4-5 Transaction injector
시뮬레이션될 비트코인 네트워크에 트랜잭션 생성을 주기적으로 해줄 것인지에 대한 옵션을 선택하는 콘솔임. 선택지는 활성/비활성화가 있음. <br><br> 

#### 4-6 Number of transactions
시뮬레이션될 비트코인 네트워크에 얼마나 트랜잭션을 생성할지 결정하는 콘솔임. 4-5 콘솔에서 "enable" 입력을 받아야지, 이 콘솔이 활성화됨. 생성할 트랜잭션은 무조건 자연수만 입력을 받게되어 있으며, 단 -1을 입력 받을시 트랜잭션을 무한개 생성하는 옵션 값임. <br><br>

#### 4-7 Amount of bitcoin transferred
시뮬레이션될 비트코인 네트워크에서 보낼 트랜잭션에 얼만큼의 비트코인을 보낼 지에 대한 값을 결정하도록 입력 받는 콘솔임. 입력 값은 양의 정수만 가능함. 또한  4-6에서 "-1" 옵션으로 무한개의 트랜잭션을 생성하겠다는 옵션이 있는 경우에는 이 콘솔은 비활성화 됨. <br><br>

#### 4-8 Transaction interval
시뮬레이션될 비트코인 네트워크에서 전송될 트랜잭션의 전송 주기를 입력 받는 콘솔. 입력 값은 무조건 양의 정수이며, 4-6에서 "-1" 옵션으로 무한개의 트랜잭션을 생성하겠다는 옵션이 있는 경우에는 이 콘솔은 비활성화 됨. <br><br>

# 5. Simulation result

## Console output
```
python3 start_emulation.py
>
Input node count (only integer more than 1) : 1
Input simulation time (sec) : 10
Input mining algorithm(pow/coinflip) : coinflip
Input difficulty(1/2/3) : 3
Input transaction injector (enable/disable) : enable
input number of transcations ( -1 : infinite number ): 1
Enter only one of them (enable/disable) 
input number of Bitcoins transferred (minimum amount : 0.0000546) : 1
Input transaction interval (sec) : 1
start make_approximate_setmining_test.py start 
enable is start  
generating xml is finished 
---------------------------------------------------------------------------------------
Success xml existence test ...
shadow running ... 
Success blockchain test output file existence ...
Success transaction test ...
Success transaction test ... 
test result : 1/1 
Success shadow test ...
Success test_walletAddress  ... 
test result : 1/1 
emulation success!!

```

## shadow output
시뮬레이션이 성공적으로 끝나면 shadow.data 디렉토리가 생성될 것이며, shadow.data 디렉토리 구조는 다음과 같음.이 중 output.txt는 shadow console 로그를 담고있는 파일임. 이 파일을 통해 xml에 정의된 플러그인이 제대로 시뮬레이션이 되었나 확인을 할 수 있으며, 시뮬레이션을 전체적으로 검증하는데 유용함.
```
shadow.data
├── hosts
│   ├── bcdnode0
│   └── client0
├── output.txt
├── peers
│   └── result_19:44:41_0.log
└── total_result.txt

```
##  plugin(bitcoin) output
비트코인 플러그인의 실행 로그를 보여주는 파일임. 일반 bitcoind를 실행 했을때 터미널 콘솔로 print되는 로그들임. 이 로그를 통해서 해당 비트코인 플러그인의 동작을 검증을 할 수 있음. <br><br>

## plugin(rpc) output
비트코인 플러그인의 rpc function request 플러그인임. 플러그인에는 "getnewadress", "validateaddress", "setgeneratetoaddress", "getmempoolinfo" 이렇게 4개의 rpc function request가 정의가된 플러그인임. 이 플러그인은 rpc response를 받은 결과가 print되어 이렇게 로그로 결과가 기록이됨.
```
cat stdout-client0.client.1000.log
>
request : {"id":"rpc_client","jsonrpc":"1.0","method":"getnewaddress","params":[]}
-- result : {"result":"37JeDrvKyr5DDvgE9ysjQahdnnQaPu97tm","error":null,"id":"rpc_client"}

wallet:37JeDrvKyr5DDvgE9ysjQahdnnQaPu97tm
request : {"id":"rpc_client","jsonrpc":"1.0","method":"validateaddress","params":["37JeDrvKyr5DDvgE9ysjQahdnnQaPu97tm"]}
-- result : {"result":{"isvalid":true,"address":"37JeDrvKyr5DDvgE9ysjQahdnnQaPu97tm","scriptPubKey":"a9143d957cf4e29dbd6cae0bc933b25d26b5b4322cb387","isscript":true,"iswitness":false},"error":null,"id":"rpc_client"}

request : {"id":"rpc_client","jsonrpc":"1.0","method":"setgeneratetoaddress","params":["37JeDrvKyr5DDvgE9ysjQahdnnQaPu97tm"]}
-- result : {"result":true,"error":null,"id":"rpc_client"}

request : {"id":"rpc_client","jsonrpc":"1.0","method":"getmempoolinfo","params":null}
-- result : {"result":{"loaded":true,"size":0,"bytes":0,"usage":0,"maxmempool":300000000,"mempoolminfee":0.00001000,"minrelaytxfee":0.00001000},"error":null,"id":"rpc_client"}

```

## total_result.txt

다음은 total_result.txt의 결과 파일임 보면, 마지막 블록 해시 값들의 일치율과 실행되는 동안 소모된 실제 시간과 소모된 시뮬레이션 시간과, 선택한 합의 알고리즘, 노드의 연결 상태에 대해 다음과 같이 결과를 보여줌.
```
---------------------------------------------------------------------------
					 total result 
---------------------------------------------------------------------------
	1. Last block hash match rate : 1/1
	2. real runtime : 00:00:04
 
	3. simulation runtime : 10 sec
	4. Algorithm : coinflip 
	4. peer connection status : 
```

## Each node's result
생성된 노드 개수 만큼 peers 디렉토리에 생성될 txt파일임. 현재 1개 노드만 시뮬레이션을 하였음. 이 결과 파일을 통해, 다음과 같은 정보를 확인할 수 있음.

```
---------------------------------------------------------------------------------
1. node 갯수 : 1
2. simulation time : 10 sec
3. 생성된 블록 개수 : 3
4. 마지막 블록의 hash 값 : 54293afe8ad9d6681eabc2b1bdd5f610a560a553dfe50dd9ba295fcc9bbe07c2
5. 생성된 트랜잭션 개수 : 3
6. TPS : 0.3
---------------------------------------------------------------------------------
7. Blockhash list
	7-1 blockhash : 000006d66f3cd0f0e2bf5ff0f3aa66bf4deec8eff96b7bc13fc7622532b10ad8
	7-2 blockhash : 993c9b4d0c145c168943fa978fab50e4b166fc1969baf8ed0af8d9861ef2c272
	7-3 blockhash : 54293afe8ad9d6681eabc2b1bdd5f610a560a553dfe50dd9ba295fcc9bbe07c2

```
# 6. Example

### Simulation condition
Node : 5 <br>
Network topology : linear (default)<br>
Simulation time : 20 sec <br>
Mining algo : coinflip <br>
Transaction generator : disable <br>
Block difficulty : 3 <br>

### Run
```
python3 start_emulation.py
>
Input node count (only integer more than 1) : 5
Input simulation time (sec) : 20
Input mining algorithm(pow/coinflip) : coinflip
Input difficulty(1/2/3) : 3
Input transaction injector (enable/disable) : disable
```

### console output
```
start make_approximate_setmining_test.py start 
generating xml is finished 
---------------------------------------------------------------------------------------
Success xml existence test ...
shadow running ... 
Success blockchain test output file existence ...
Success shadow test ...
Success test_walletAddress  ... 
test result : 5/5 
emulation success!!

```

### shadow output

```
├── hosts
│   ├── bcdnode0
│   ├── bcdnode1
│   ├── bcdnode2
│   ├── bcdnode3
│   ├── bcdnode4
│   ├── client0
│   ├── client1
│   ├── client2
│   ├── client3
│   └── client4
├── output.txt
├── peers
│   ├── result_10:12:37_0.log
│   ├── result_10:12:37_1.log
│   ├── result_10:12:37_2.log
│   ├── result_10:12:37_3.log
│   └── result_10:12:37_4.log
└── total_result.txt

```

### Total output

```
---------------------------------------------------------------------------
					 total result 
---------------------------------------------------------------------------
	1. Last block hash match rate : 5/5
	2. real runtime : 00:00:32
 
	3. simulation runtime : 20 sec
	4. Algorithm : coinflip 
	4. peer connection status : 

		 node : 1.0.0.1  ---------- connection match rate : 2/5 
				 - 1.1.0.1 
				 - 1.4.0.1 
		 node : 1.1.0.1  ---------- connection match rate : 3/5 
				 - 1.0.0.1 
				 - 1.2.0.1 
				 - 1.3.0.1 
		 node : 1.2.0.1  ---------- connection match rate : 2/5 
				 - 1.1.0.1 
				 - 1.3.0.1 
		 node : 1.3.0.1  ---------- connection match rate : 3/5 
				 - 1.1.0.1 
				 - 1.2.0.1 
				 - 1.4.0.1 
		 node : 1.4.0.1  ---------- connection match rate : 2/5 
				 - 1.0.0.1 
				 - 1.3.0.1 

```