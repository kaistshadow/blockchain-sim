이 문서에서는 Bleep Emulation에 대해서 이야기하고자 한다.    
BLEEP 프로젝트의 주요 목표인 블록체인의 성능 측정을 하기 위해서는 BLEEP emulation이 전제 조건이 되어야 한다.    
BLEEP emulation의 주된 기능은 사용자가 입력한 시간 만큼 실행하고, 그에 대한 블록체인 성능을 수치로 표현한다.     

# Plugins
BLEEP emulation을 하기 위해서는 네 가지 종류의 플러그인이 필요하다. 
1. Bitcoin Application: 시뮬레이션 할 블록체인 실행 파일(ex) 비트코인)
2. Client: 블록체인의 현재 상태와 메인체인에 대한 정보를 알고 있으며, 전체 노드 중 1개만 속한다. 
3. Tx Injector: 실행중인 블록체인 노드에게 트랜잭션을 생성하는 rpc 콜을 날리는 역할을 진행   

이러한 플러그인들은 사용자가 xml 파일에 정의한 대로 shadow를 통해 실행되며, 해당 xml 파일은 사용자가 입력한 파라미터를 xml Generator를 사용하여 생성해준다. 

# Process
일반적인 Emulation은 다음과 같이 진행 된다.
1. 사용자의 요구사항에 맞게 xml 파일 생성
2. xml 파일을 기반으로 shadow 실행 
3. 블록체인 정보 및 시뮬레이션 결과 요약

# Install
시뮬레이션을 실행 하기 위해서는 플러그인의 .so 파일이 필요 하다. 
다음을 따라 설치 하면 된다.(전단계에서 설치 하셨으면 이번단계 불필요)
```
cd blockchain-sim
python setup.py --bitcoin
source ~/.bashrc
```

# Run
xml 파일 생성 부터 실행 결과 출력까지 한번에 하고자 하면 다음과 같이 입력하면 된다. 
```
cd blockchain-sim
cd BLEEPemul
cd emulation
pyhton3 start_emulation.py
```
실행 하게 되면, 다음과 같은 파라미터들의 입력이 필요하다.
* 시뮬레이션 시간: 실험하고자 하는 시뮬레이션 시간 입력, 단위는 초
* 알고리즘(pow/coinflip): 비트코인의 합의 알고리즘을 하드웨어를 사용하여 마이닝 하는 pow 알고리즘과, pow 알고리즘을 추상화 시켜 컴퓨팅 파워의 소모 없이 합의 하는 coinflip 알고리즘을 선택하여 사용 가능 
* 난이도(1/2/3): 비트코인의 genesis block의 마이닝 난이도를 지정. 3개의 난이도로 나뉘어 지며, 1(mainnet과 동일 난이도), 2(mainnet과 testnet의 중간 난이도), 3(testnet과 동일 난이도)
* 트랜잭션 generator (enable/disable): 트랜잭션을 생성하는 tx generator의 사용 여부 
   * cnt: 트랜잭션 생성 개수( 개수가 -1 이면 max TPS 측정)
   * interval: 다음 트랜잭션까지의 간격, 단위는 초
   * amount: 트랜잭션에서 보낼 금액, 단위는 BTC  

시뮬레이션을 실행 하고 나면, 실험 결과 로그가 datadir에 폴더에 쌓인다.
* bcdnode/ : 각 비트코인 노드의 실행 로그 
* client/ :client에 관한 로그 
* txinjector/ : tx injector가 실행 되었을 경우에 생성된 트랜잭션에 
* result.log : 전체 블록체인 시뮬레이션 결과 요약

각각의 프로세스를 실행하려면 다음과 같이 실행하면 된다.

### xml generator
```
cd blockchain-sim/BLEEPemul/emulation
python xmlGenerator.py 1 [sim_time] [algorithm] [difficulty] [tx_mode] [tx_cnt] [tx_sec] [txamount]
```

### shadow 실행 
shadow를 실행 하기 위해서는 정의된 xml 파일이 존재 해야 한다. 
```
cd blockchain-sim/BLEEPemul/emulation
shadow -d datadir output.xml
```
shadow에는 여러가지 옵션이 존재한다.
* -d: 데이터 디렉토리 경로 지정 (필수)
* -w: 비트코인 마이닝 시에 사용하는 worker thread 개수

### result 파일 생성
(현재 지원하지 않는 기능, 생성할 예정)
 

