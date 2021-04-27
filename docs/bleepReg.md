# Regression test <br>
regression test는 지원하는 각 버전의 기능에 대한 테스트를 진행하여 해당 기능이 정상적으로 동작하는지 확인하고자 한다.<br>  
이중에서도 이 문서에서는 단일 노드의 비트코인(release 0.1.0)에 대한 테스트 방법에 대해서 소개하고자 한다.<br>
Bitcoin Regtest 폴더는 blockchain-sim/tests/regtest 에 존재한다.<br>

# Install & Run 
tests/regtest 폴더에 존재하는 모든 테스트들을 실행 하기 위해서는 다음과 같이 실행 하면 된다. 

## Install
```
cd blockchain-sim; python setup.py --bitcoin
```


## Run
```
python setup.py --test 
```
<br>

# Bitcoin Regtest 
Bitcoin regtest는 비트코인의 기능을 테스트 해 주는 역할을 한다. 
만약 success에 실패 하였으면, 왜 실패 했는지 이유를 전달 하는 역할을 하기도 한다. 

지원 하는 bitcoin regtest의 종류는 다음과 같다. 
  + xml 파일 생성 test 
  + shadow 실행 test
  + bitcoin 실행 test
  + bitcoin 난이도 조정 test
  + bitcoin wallet address test
  + bitcoin mining test
  + bitcoin main chain test
  + bitcoin transaction 생성 test
  + bitcoin transaction count test
  + bitcoin initial coin test <br><br>


# 1. xml 파일 생성 test <br>
시뮬레이션을(shadow)를 진행 하기 위해서는 각 플러그인이 정의되어 있는 xml 파일이 필요 하다.<br>
xml 파일 생성 test는 xml generator가 제대로 xml 파일을 생성했는지 체크 하는 테스트이다.<br>   
xml generator는 사용자로부터 입력받은 파라미터들을 사용하여 xml 파일을 만들고, 해당 xml 파일이 존재 하는지 확인한다.<br>   

test를 실행 하기 위하여 다음과 같이 동작 한다.<br>
```bash
cd blockchain-sim/tests/regtest/1_bitcoin/1_xmlGenerate
python3 xmlGen_test.py
```
실험을 하고자 하려면 xml 파일을 생성해야 한다. 
xml 파일 생성 파라미터는 다음과 같다. 
* 시뮬레이션 시간
* 알고리즘(pow/coinflip)
* 난이도(1/2/3)
* 트랜잭션 generator (enable/disable)
   * disable
   * enable
      * tx cnt 개수가 -1 이면 max TPS 측정
      * tx interval
      * tx amount
      <br>

파라미터에 관한 자세한 내용은 1. BLEEP emulation을 참고 하면 된다.

<br>

>xml 파일 생성 test를 제외한 비트코인 regtest에서는 시뮬레이션으로 얻은 결과를 바탕으로 result를 작성하므로, 미리 작성되어 있는 xml 파일(output.xml)을 사용하여 시뮬레이션을 진행한다.<br>
새로운 xml 파일을 실행하고자 할 때는 --regtest 옵션을 실행 하면 xml 생성 단계부터 시작하게 된다.  
xml 생성시 입력 파라미터는 1_xml 파일생성 test와 동일하다. 
```
cd blockchain-sim/tests/regtest/[테스트 번호_테스트이름]
python3  [테스트이름].py --regtest 
```

<br>

# 2. shadow 실행 test <br>
shadow 실행 test는 xml 파일에 정의한 플러그인들이 shadow를 통해서 정의한 시간에 실행 되고, 종료 되었는지 확인하고자 한다.    
shadow 실행 후, 시뮬레이션의 결과로 출력하는 output.txt 파일의 존재 유무와 파일의 로그 데이터를 분석하여 xml에 정의한 플러그인의 실행 여부를 판단한다.

test를 실행 하기 위하여 다음과 같이 동작 한다.<br>
```bash
cd blockchain-sim/test/regtest/2_shadow
python3 shadowCheck.py
```

<br>

# 3. bitcoin 실행 test
bitcoin 실행 test는 xml에 정의 되어 있는대로 비트코인 노드가 실행 되는지 테스트 하기 위함이다.  
xml에 정의한 비트코인의 노드의 속성인 port, datadir, rpc address가 정의 대로 동작 하는지 판단한다.  
* bitcoin log(shadow.data/bcdnode0/std-bitcoin.log )와 xml 파일의 속성  비교 

test를 실행 하기 위하여 다음과 같이 실행한다.<br>
<br>
```bash
cd blockchain-sim/tests/regtest/1_bitcoin/3_blockchainApplication
python3 bitcoinApplication.py
```
<br>

# 4. bitcoin 난이도 조정 test
비트코인 시뮬레이션 진행 시 마이닝 난이도를 조절 할 수 있다.  
현재 시뮬레이션 에서는 3가지 종류의 마이닝 난이도를 지원하고 있다(1/2/3).
* 1: 비트코인 mainnet 난이도와 동일 (10분에 블록 1개 생성) 
* 2: 비트코인 mainnet과 testnet 난이도의 중간 (10분에 블록 1개 생성 )
* 3: 비트코인 testnet 난이도와 동일 ()

시뮬레이션 실행 시 xml 파일에 정의한 난이도대로 실행 되고 있는지 확인한다.  
xml에 정의한 비트코인 마이닝 난이도와 그에 해당 하는 bitcoin의 difficulty 수치가 일치 하면 테스트는 성공 한다. 

test를 실행하기 위하여 다음과 같이 진행 한다.
```bash
cd blockchain-sim/tests/regtest/1_bitcoin/4_difficulty 
python3 difficulty.py
```
<br>


# 5. bitcoin wallet address test
현재 비트코인에서는 wallet을 생성 하기 위하여 client 플러그인을 사용하고 있으며, client는 rpc call을 사용하여 wallet address를 생성 한다.  
이러한 방식으로 생성된 wallet address에 오류가 없는지 판단 하기 위하여 wallet address test 를 진행 한다.  
wallet address test는 다음과 같은 환경에서 이루어 진다.  
* shadow 실행 시 비트코인이 실행 된다.
* client는 실행되고 있는 비트코인에게 wallet 주소를 입력 해 준다. 
* 전달받은 비트코인은 rpc client에 따라서 wallet address를 만든다

여기서 체크해야 할 점은 
1. rpc client의 response message 전달 받음(return 된 값이 address)
2. 비트코인에서 rpc call 호출 된 로그 확인 
```
cd blockchain-sim/tests/regtest/1_bitcoin/5_walletAddress
pyhton3 walletAddress.py
```

<br>

# 6. bitcoin mining test
시뮬레이션이 실행 되면 비트코인은 genesis block을 생성하고, client가 mining rpc call을 호출하여 마이닝 하도록 해준다.  
해당 테스트는 비트코인 마이닝이 진행 되는지 체크 하기 위함이며, genesis block 이외에 마이닝된 블록이 있는지 확인한다.  
시뮬레이션 종료 전 블록 개수가 0 이상(genesis block 제외)이면 마이닝에 성공했다고 간주한다. 
```bash
cd blockchain-sim/tests/regtest/1_bitcoin/6_mining; 
python3 mining.py
```

<br>


# 7. bitcoin main chain test
시뮬레이션 후 메인체인의 마지막 블록을 정확히 표현 했는지 확인하기 위하여 main chain test를 실행 한다.  
메인체인의 마지막 블록 hash 값이 rpc client와 비트코인 로그가 동일한지 확인한다. 
* 비트코인 노드는 마이닝을 실행 해야 한다. 
* client 노드는 시뮬레이션의 마지막에 메인체인의 정보를 확인하는 getblockchaininfo rpc call을 생성한다. 

이런 동작 과정에서 client 로그에 찍힌 besthashblock 과 bitcoin log의 가장 마지막 block의 hash값이 동일한지 비교 한다.

```bash
cd blockchain-sim/tests/regtest/1_bitcoin/7_mainchain; 
python3 mainchaintest.py
```
<br>

# 8. bitcoin transaction 생성 test
비트코인 실행 시 트랜잭션을 생성 해주는 역할을 하는 것이 tx generator 이다. 
tx generator는 실행중인 비트코인에 rpc call을 호출하여 트랜잭션을 생성하도록 한다. 

xml 파일에 정의한 트랜잭션 개수만큼 생성되었는지 확인하기 위하여 bitcoin transaction 테스트를 진행한다. 


```bash
cd blockchain-sim/tests/regtest/1_bitcoin/8_transaction;
python3 transaction.py
```

xml 생성시 주의 할 점:  
해당 테스트는 transaction을 사용하는 테스트 이므로 트랜잭션을 생성 해주는 transaction generator가 enable 되어 있어야 한다.


<br>


# 9. bitcoin transaction count test

비트코인에서 TPS 측정을 위해서 메인체인의 블록에 포함된 개수를 센다.  
비트코인이 실행 될 때 트랜잭션을 생성 해주는 rpc call을 tx generator가 호출 하고, 
호출받은 비트코인은 해당 트랜잭션을 블록에 담아 마이닝을 성공 한 후 메인체인에 연결 한다. 

이때, 생성한 트랜잭션 개수와 비트코인에서 생성한 트랜잭션 개수가 동일한지 파악한다.
* Tx generator는 xml 파일에서 트랜잭션 개수를 지정하였고, rpc call의 response가 에러가 없어야 한다. 
* 비트코인의 로그를 파싱하여 메인체인에 포함된 트랜잭션 개수와 txpool에 포함되어 있는 개수를 확인한다.
* tx generator 생성한 개수 + 블록 개수(coinbase tx)  == 메인체인에 포함된 트랜잭션 개수 + tx pool에 포함된 트랜잭션 개수 비교 

```bash
cd blockchain-sim/tests/regtest/1_bitcoin/9_transactionCheck 
python3 transaction_count.py
```

xml 생성 시 주의 할 점:  
--regtest 옵션을 사용하여 xml을 생성 할 때, transaction generator는 enable 상태여야 한다.  
* xml 생성시에 트랜잭션 개수가 -1 이면 max TPS를 측정하는 것이므로 정확한 측정 불가(무한대로 트랜잭션을 생성하기 때문)  
* 따라서, xml 생성시에 트랜잭션의 개수는 유한하여아 함 

<br>

# 10. bitcoin initial coin test
비트코인에서 트랜잭션을 생성하기 위해서는 coinbase wallet에 사용할 수 있는 금액의 비트코인이 필요하다.  
이러한 것을 위하여 genesis block을 제외한 첫번째 블록의 genesis block의 coinbase를 증가 시켜 주었다.  
또한, 100블록 이후에 사용할 수 있던 UTXO들을 6블록 이후에 사용할 수 있도록 하여, 빠른 시간 안에 트랜잭션 생성이 가능하도록 하였다. 

이러한 기능이 정상적으로 동작하는지 파악 하기 위하여 bitcoin initial coin test를 작성 하였다.
test는 mining rpc request가 없음에도 블록이 생성이되었는지 확인한다. 

```bash
cd blockchain-sim/tests/regtest/1_bitcoin/10_initialCoin
python3 initialCoin.py
```
<br>

# 11. peer connection test
블록체인은 분산 원장 데이터베이스 개념으로서, 시스템을 유지하기 위해서는 네트워크에 참여하는 노드의 수가 많을 수록 안정성과 확장성 측면에서는 좋다. 그렇기에 시뮬레이션 환경에서 또한 복수의 노드들을 시뮬레이션 하는 것이 반드시 필요하며, 정의된 노드들 끼리도 연결이 반드시 되어야 한다. 이런 환경을 시뮬레이션 하기 위해서는 필요한 정보들을 xml 파일에 정의를 한다. 필요한 정보는 노드의 개수, 노드들 간의 connection 정의이다. 

```
  <plugin id="bitcoind" path="BITCOIND.so"/>
  <node id="bcdnode0" iphint="1.0.0.1">
    <application arguments=""/>
  </node>
  <node id="bcdnode1" iphint="1.1.0.1">
    <application arguments=""/>
  </node>
```
시뮬레이션을 하기 위해서는 다음과 같이 시뮬레이션 상에서 실행 시킬 노드의 프로그램 파일 (shadow plugin)은 xml에서 node와 함께 정의가 된다. 이렇게 node와 함께 정의가된 bitcoind 시뮬레이션 노드를 추가하고 싶으면, 계속해서 xml파일에 정의를 하면 된다.
```
  <node id="bcdnode1" iphint="1.1.0.1">
    <application arguments="-addnode=1.0.0.1:18333"/>
  </node>
```
정의가 끝나면, application arguments 에서 블록체인에서 지원하는 node connection flag 값을 정의를 해준다. 위의 예시는 bitcoin의 flag값으로서 "addnode" flag를 통해 노드간의 연결을 정의를 한다. 
<br><br>
```
cd blockchain-sim/tests/regtest/1_bitcoin/11_peerConnection;
python3 peerConnection.py
```
peer connection test를 진행 하기 위해서는 다음 명령어를 통해 test를 해볼 수 있다. peer connection test를 통해서 xml 파일에 정의된 노드들의 정보과 connection 정보대로 시뮬레이션이 된지 확인을 할 수 있다. <br>

# 12. dumpfileTest
TPS 테스트를 원활히 하기 위해서는 bitcoin rule 중에 101번째 블록 이후에 보상된 bitcoin에 의해 생성된 트랜잭션에 대해서만 정당성을 주는 rule을 없애야 한다.또한 비트코인을 많이 갖고있는 wallet과 database들을 사전에 정의를 하여 시뮬레이션 실행마다 로드를 해주어야 한다. 이런 셋팅이 끝나면 시뮬레이션 시작과 동시에 TX를 생성할 수 있게 된다. 이렇게 원활한 TPS 테스트를 위해 사전에 정의된 파일들을 미리 dump 파일로 만들어서 시뮬레이션 마다 해당 dump 파일을 사용하기 위해서는 bitcoin application flag에 "reindex" 값이 정의가 되어야 한다. 
```
    <application arguments="-reindex />
```
해당 dump file test는 "Reindexing finish" 로그가 print 되며, dump file의 블록 hash 값들과 bitcoin 플러그인의 로그 값을 비교한다. dump file test를 진행하고 싶으면 다음 명령어를 통해 확인할 수 있다.
```
cd blockchain-sim/tests/regtest/1_bitcoin/12_dumpfileTest;
python3 dump.py
```

# 13. Monitor Node connection Test
시뮬레이션의 결과를 효율적이게 관리를 하려면 별도로 데이터만 관리 하는 노드가 필요하다. 이를 모니터 노드라고 하며, 모니터 노드는 네트워크에 선언된 모든 노드와 연결을 유지한다. 해당 테스트에서는 모니터 노드와 그 외에 노드들 간의 연결이 제대로 이루어지는지 확인하기 위한 테스트이다.
```
cd blockchain-sim/tests/regtest/1_bitcoin/13_monitorNode;
python3 monitor.py
```
다음 명령어를 통해 모니터 노드가 블록체인 노드와 연결이 이루어졌는지 확인을 할 수 있다. <br>

# 14. Block propagation Test
모니터 노드가 효율적이게 관리하는 데이터는 결국 block hash 값이다. Test-13에서 bitcoin 노드들과 모니터 노드간의 연결을 확인을 했다. 연결이 되었으면, 모니터 노드는 bitcoin 노드들로 부터 생성된 블록의 hash 값을 메시지로 받는다. 테스트는 모니터 노드가 수신한 block hash값과 bitcoin 노드가 생성한 block hash 값과 비교를 한다. 실행은 다음과 같다.
```
cd blockchain-sim/tests/regtest/1_bitcoin/14_propagationBlock;
python3 block_propagation.py
```
<br>

# 15. Transaction generator connection test
TPS를 효율적으로 측정하기 위해서는 bitcoin 노드 이외에 별도의 transaction generator가 필요하다. transaction geneator는 transaction을 전파하기 위해서 bitcoin 노드들과 연결이 되어야 한다. 본 테스트 이러한 bitcoin 노드들과 transaction generator가 연결이 되었는지 확인을 하는 테스트다.
```
cd blockchain-sim/tests/regtest/1_bitcoin/15_txGenerator_connection;
python3 txGenerator.py
```