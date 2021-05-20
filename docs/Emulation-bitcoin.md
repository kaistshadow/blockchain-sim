
비트코인 어플리케이션을 에뮬레이션하기 위해서는 `Shadow`시뮬레이터, `plugins`, `xml`파일이 필요하다. <br>
BLEEP을 설치하면 에뮬레이션에 필요한 파일이 아래의 경로에 각각 설치 된다. 
- `shadow` : `~/blockchain-sim/shadow`
- `plugin` : `~/blockchain-sim/Install/plugins` 또는 `~/blockchain-sim/<해당 테스트케이스 폴더>`
- `xml` 파일 : `~/blockchain-sim/<해당 테스트케이스 폴더>`

`shadow`는 네트워크 및 비트코인의 동작을 시뮬레이션 및 에뮬레이션하는 역할을 담당한다.<br> 
`plugin`은 `shadow`에서 네트워크에 참여하는 시뮬레이션된 노드를 뜻한다.<br> 
`xml` 파일은 시뮬레이션할 네트워크 토폴로지와 플러그인이 정의되어 있고 시뮬레이션이 실행되면 `xml`에서 설정한 값에 따라 동작하게된다.<br>

BLEEP은 비트코인 에뮬레이션 실험을 위한 스크립트 코드(`BLEEPemul/emulation/start_emulation.py`)를 제공한다.
실험을 위한 `plugin`인의 구성, `xml` 파일 정의 그리고 스크립트 코드를 이용한 실험 방법은 아래와 같다. 

# Plugin Guides
비트코인 에뮬레이션을 위한 주요 구성은 다음과 같다. 자세한 사용법은 shadow에서 제공하는 [매뉴얼](https://github.com/shadow/shadow/blob/main/docs/2-Getting-Started-Tutorial.md#configuration)을 참고바란다. 

### Logistics 
- `shadow` 프로세스는 `xml`파일에 정의된 내용을 기반으로 네트워크 및 노드를 구성한다. 
- 비트코인 노드(`libBITCOIND_0.19.1DEV.so`)가 초기화 및 실행된다. 
- 채굴관련 노드(`rpc.so`)가 초기화 및 실행된다. 
	- `rpc.so`는 채굴 노드를 실행 하기 위해 필요한 `plugin`이다. 이 `plugin`이 실행하게 될 rpc request는 다음과 같다.
	```
	  std::string wallet = rpc_request_with_no_params("getnewaddress");
	  rpc_reqeust_with_params("validateaddress", params_list);
	  rpc_reqeust_with_params("setgeneratetoaddress", params_list);
	  rpc_request_no_return_no_params("getmempoolinfo");
	  rpc_request_no_return_no_params("getblockchaininfo");
	  rpc_request_no_return_no_params("getpeerinfo");
	```
- 트랜잭션 관련 노드(`transaction.so`)가 초기화 및 실행된다. 
	- `transaction.so`는 트랜잭션 관련 노드를 실행하기 위해 필요한 `plugin`이다. 이 `plugin`이 실행하게 될 rpc request는 다음과 같다.
	```
    while(blockcnt <= 6) { //비트코인 노드는 블록 101개가 아닌 블록이 6개 이상일 경우 트랜잭션 생성이 가능하도록 수정됨
        rpc_request_no_return_no_params("getblockchaininfo");
        blockcnt = json_resp["result"]["blocks"].asInt();
        sleep(1);
    }	
    ...
    for(int i = 0; i <= txcnt; i++) {
        rpc_sendtoaddress(argv[1], wallet, i, amount);
        sleep(interval);
    }
	```

### Core Components
- `libBITCOIND_0.19.1DEV.so` : `../../external/bitcoin/0.19.1dev/src`에 의해 생성되는 `bitcoind`를 shared object 형식으로 빌드함
- `rpc.so` : `one_node_setmine.cpp`, `../../testlibs/rpc_client.cpp`로 구성됨, 각 비트코인 노드에게 블록을 채굴하도록 지시하는 노드
- `transaction.so` : `transaction.cpp`, `../../testlibs/rpc_client.cpp`로 구성됨, 특정 비트코인 노드에게 트랜잭션 생성 및 전송하도록 지시하는 노드

# Configuration Guides
`shadow` 시뮬레이터는 개발자가 실시간으로 동작을 제어할 수 없다. 즉, 사전에 컨트롤이 필요한 부분을 `xml` 파일에 미리 정의하여 사용해야 한다. <br>
추가적으로 비트코인은 다양한 [실행 플래그 옵션](https://gist.github.com/frz-dev/5390b954f9cfe3c8daa757c6c8da92ef)이 있으니 참조바란다. 

- `plugin` 정의 및 사용법
```
<!-- 비트코인 plugin 정의 --> 
  <plugin id="bitcoind" path="libBITCOIND_0.19.1DEV.so"/>  
<!-- 채굴관련 plugin 정의 -->   
  <plugin id="client" path="rpc.so"/>  
<!-- 트랜잭션 관련 plugin 정의 -->   
  <plugin id="txInjector" path="transaction.so"/>  
<!-- 비트코인 노드 정의 -->   
  <node id="bcdnode0" iphint="1.0.0.1">
    <application arguments="-debug -reindex -datadir=data/bcdnode0 -port=18333 -txindex=1 -fallbackfee=0.0002 -rpcuser=a -rpcpassword=1234 -rpcport=11111 -rpcallowip=1.0.0.1/0 -rpcb
ind=1.0.0.1 -addnode=1.9.0.1:18333 -addnode=1.1.0.1:18333 -algorithm=pow -difficulty=3" plugin="bitcoind" time="0"/>
  </node>
  ...
<!-- 채굴관련 노드 정의 -->   
  <node id="client0">
    <application arguments="1.0.0.1:11111 53" plugin="client" time="5"/>
  </node>
  ... 
<!-- 트랜잭션 관련 노드 정의 -->   
  <node id="injector">
    <application arguments="1.0.0.1:11111 10 10 5.46e-05" plugin="txInjector" time="5"/>
  </node>
```

# Script tool Guides 

- __스크립트 도구 사용 방법__ <br>
BLEEP에서 제공하는 스크립트 도구인 `BLEEPemul/emulation/start_emulation.py`을 이용해 비트코인 노드를 에뮬레이션하면 아래와 같은 화면이 출력된다. 
	```
	cd BLEEPemul/emulation; python3 start_emulation.py
	>
	3-1 Input node count (only integer more than 1) : 
	3-2 Input simulation time (sec) : 
	3-3 Input mining algorithm(pow/coinflip) : 
	3-4 Input difficulty(1/2/3) : 
	3-5 Input transaction injector (enable/disable) :       
	3-6 input number of transcations ( -1 : infinite number ): 
	3-7 input number of Bitcoins transferred (minimum amount : 0.0000546) : 
	3-8 Input transaction interval (sec) : 
	```

	> #### 3-1 Node count
	> 시뮬레이션할 노드의 개수를 입력하는 콘솔임. 무조건 자연수만 입력을 받음. 
	> #### 3-2 Simulation time
	> 시뮬레이션할 시간을 입력받는 콘솔임. 무조건 자연수만 입력을 받음. 
	> #### 3-3 Mining algorithm
	> 시뮬레이션될 비트코인의 Mining algorithm을 선택하는 콘솔임. 기존의 알고리즘인 pow와, pow 블록 생성 로직을 추상화하여 복잡한 연산을 생략시키고, 포화성 분포로 블록 생성 알고리즘을 대체한 알고리즘인 coinflip 중에 입력을 받음.
	> #### 3-4 Network Difficulty
	> 시뮬레이션될 비트코인의 블록생성 시간에 직접적인 영향을 주는 난이도의 단계를 선택하는 알고리즘임. 총 3단계로서 난이도와 단계는 반비례 관계임. 즉 1단계 난이도가 가장 어려우며, 3단계 난이도가 가장 쉬운 난이도임. 
	> #### 3-5 Transaction injector
	> 시뮬레이션될 비트코인 네트워크에 트랜잭션 생성을 주기적으로 해줄 것인지에 대한 옵션을 선택하는 콘솔임. 선택지는 활성/비활성화가 있음.
	> #### 3-6 Number of transactions
	> 시뮬레이션될 비트코인 네트워크에 얼마나 트랜잭션을 생성할지 결정하는 콘솔임. 3-5 콘솔에서 "enable" 입력을 받아야지, 이 콘솔이 활성화됨. 생성할 트랜잭션은 무조건 자연수만 입력을 받게되어 있으며, 단 -1을 입력 받을시 트랜잭션을 무한개 생성하는 옵션 값임. 
	> #### 3-7 Amount of bitcoin transferred
	> 시뮬레이션될 비트코인 네트워크에서 보낼 트랜잭션에 얼만큼의 비트코인을 보낼 지에 대한 값을 결정하도록 입력 받는 콘솔임. 입력 값은 양의 정수만 가능함. 또한  3-6에서 "-1" 옵션으로 무한개의 트랜잭션을 생성하겠다는 옵션이 있는 경우에는 이 콘솔은 비활성화 됨. 
	> #### 3-8 Transaction interval
	> 시뮬레이션될 비트코인 네트워크에서 전송될 트랜잭션의 전송 주기를 입력 받는 콘솔. 입력 값은 무조건 양의 정수이며, 3-6에서 "-1" 옵션으로 무한개의 트랜잭션을 생성하겠다는 옵션이 있는 경우에는 이 콘솔은 비활성화 됨. 

- __The logs__
	- Console output
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

	- shadow output <br>
		시뮬레이션이 성공적으로 끝나면 shadow.data 디렉토리가 생성될 것이며, shadow.data 디렉토리 구조는 다음과 같음.이 중 output.txt는 shadow console 로그를 담고있는 파일임. 이 파일을 통해 xml에 정의된 플러그인이 제대로 시뮬레이션이 되었나 확인을 할 수 있으며, 시뮬레이션을 전체적으로 검증하는데 유용함.
		```
		shadow.data
		├── hosts
		│   ├── bcdnode0
		│   ├── client0
		│   └── injector
		├── output.txt
		├── peers
		│   └── result_19:44:41_0.log
		└── total_result.txt	
		```
	
		- hosts 폴더  
			- `bcdnode*` 폴더 <br>
			비트코인 플러그인의 실행 로그. `bitcoind`를 실행 했을때 터미널 콘솔로 print되는 로그. 이 로그를 통해서 해당 비트코인 플러그인의 동작을 검증을 할 수 있음. 
			- `client*` 폴더 <br>	   
			비트코인 플러그인의 rpc function request 플러그인임. 플러그인에는 "getnewadress", "validateaddress", "setgeneratetoaddress", "getmempoolinfo" 이렇게 4개의 rpc function request가 정의가된 플러그인임. 이 플러그인은 rpc response를 받은 결과가 print되어 이렇게 로그로 결과가 기록이됨.	
				```	
				cat stdout-client0.client.1000.log	
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
			- injector 폴더 <br>
			`transaction.cpp`에 정의된 트랜잭션을 생성하는 노드. 
		- output.txt <br>
		`shadow` 시뮬레이션을 실행 했을때 터미널 콘솔로 print되는 로그. 이 로그를 통해서 시뮬레이터의 동작을 검증을 할 수 있음. <br>
		- peers 폴더 <br>
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
		- total_result.txt <br>
		마지막 블록 해시 값들의 일치율과 실행되는 동안 소모된 실제 시간과 소모된 시뮬레이션 시간과, 선택한 합의 알고리즘, 노드의 연결 상태에 대해 다음과 같이 결과를 보여줌.
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


# Example experiment 

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

> ### Simulation condition
> Node : 5 <br>
> Network topology : linear (default)<br>
> Simulation time : 20 sec <br>
> Mining algo : coinflip <br>
> Transaction generator : disable <br>
> Block difficulty : 3 <br>

### output
- console output
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

- shadow output
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

- Total output
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