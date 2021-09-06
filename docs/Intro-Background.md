BLEEP을 이용하기 위해 기반이되는 배경 지식을 설명한다. 

# C/C++기반의 다양한 블록체인 어플리케이션
- BLEEP은 C/C++기반 블록체인을 에뮬레이션하기 위해 바이너리를 포팅하는 작업을 진행중이며, 다양한 디버깅 이슈 목록을 업데이트 하고 있다. <br><br>
  | 구분 | 내용 | 상태 | 비고 |
  | ------ | ------ | ------ |------ |
  | 1   | bitcoin   | done   |https://github.com/bitcoin/bitcoin|
  | 2   | monero   | done   |https://github.com/monero-project/monero|
  | 3   | zcash   | done   |https://github.com/zcash/zcash|
  | 4   | litecoin   | done   |https://github.com/litecoin-project/litecoin|
  | 5   | ripple   | progress   |https://github.com/ripple/rippled|
  | 6   | eos   | progress   |https://github.com/EOSIO/eos|
  | 7   | bitcoin-cash | progress |https://github.com/bitcoin-cash-node/bitcoin-cash-node|

# Shadow 시뮬레이터 
Shadow 시뮬레이터를 실행하기 위해서는 아래와 같이 3개의 요소(Plugin, Shadow, XML)가 준비되어야 한다. 
Shadow는 네트워크 토폴로지 및 Plugin 정의 규칙에따라 xml파일을 input값으로 받으며 실행 방법은 명령어창에 `shadow <example.xml>` 입력하면된다.<br> 
- [Shadow 시뮬레이터](https://github.com/shadow/shadow)
## Plugin
`Plugin`이란 shadow에서 네트워크에 참여하는 시뮬레이션된 노드를 뜻한다. `plugin`은 바이너리 파일 형식이 아닌 `shared object` 형식이다. 예를들어 BLEEP에서 비트코인을 시뮬레이션할 때 사용되는 플러그인은 bitcoin의 실행파일인 bitcoind 바이너리 파일이 아닌, `shared object`형식으로 변환된 `bitcoind.so` 파일이다. shadow 프로세스가 실행되면 동적으로 `Plugin`을 로드 하기 때문에 시뮬레이션할 프로그램이 `shared object` 형식이어야 한다.<br> 

## Shadow
shadow는 네트워크 동작을 시뮬레이션을 하기 위한 오픈소스 프로그램이다. shadow는 BLEEP install 과정에서 함께 설치된다.

## XML
XML파일에서 시뮬레이션할 네트워크 토폴로지와 플러그인을 정의한다. 정의된 xml파일은 shadow실행 시 argument파일로 입력이 된다. 시뮬레이션이 실행이 되면 xml에서 설정한 값에 따라 네트워크가 시뮬레이션이 된다. 
- XML 파일 작성 방법
shadow에서 시뮬레이션 될 네트워크를 설정하기 위하여 사용하는 xml파일은 다음과 같이 작성한다.
1. `<shadow>`

모든 정보는 `<shadow>` 태그 내부에 작성되는 것을 규칙으로 한다.

2. `<topology>` 태그 내부에 네트워크 토폴로지 및 연결 속성 정보 작성

`<topology>` 태그 내부에 graphml의 정보 입력 후 노드의 속성 및 그래프 연결의 속성값을 입력한다.
```
  <topology>
    <![CDATA[<?xml version="1.0" encoding="utf-8"?><graphml xmlns="http://graphml.graphdrawing.org/xmlns" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://graphml.graphdrawing.org/xmlns http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd">
             <key attr.name="packetloss" attr.type="double" for="edge" id="d9" />
             <key attr.name="jitter" attr.type="double" for="edge" id="d8" />
             <key attr.name="latency" attr.type="double" for="edge" id="d7" />
             <key attr.name="asn" attr.type="int" for="node" id="d6" />
             <key attr.name="type" attr.type="string" for="node" id="d5" />
             <key attr.name="bandwidthup" attr.type="int" for="node" id="d4" />
             <key attr.name="bandwidthdown" attr.type="int" for="node" id="d3" />
             <key attr.name="geocode" attr.type="string" for="node" id="d2" />
             <key attr.name="ip" attr.type="string" for="node" id="d1" />
             <key attr.name="packetloss" attr.type="double" for="node" id="d0" />
             <graph edgedefault="undirected">
               <node id="poi-1">
                 <data key="d0">0.0</data>
                 <data key="d1">0.0.0.0</data>
                 <data key="d2">US</data>
                 <data key="d3">10000020000040</data>
                 <data key="d4">10000240</data>
                 <data key="d5">net</data>
               </node>
               <edge source="poi-1" target="poi-1">
                 <data key="d7">50.0</data>
                 <data key="d8">0.0</data>
                 <data key="d9">0.0</data>
               </edge>
             </graph>
             </graphml>]]>
  </topology>
```
위 예시에서 data의 각 key값은 graphml의 매칭되는 id에 해당하는 정보이다. 예를 들어, `<data key="d2">US</data>`의 정보는 해당 노드의 지역 코드(`geocode`)가 US 임을 의미한다.

각 node는 가상 네트워크 상의 머신을 의미하며, id는 노드마다 고유한 값이어야 한다. 네트워크와 관련된 속성으로 패킷손실률, IP, 지역코드, 다운로드/업로드 대역폭, 네트워크 타입, ASN의 정보를 입력 할 수 있다.

각 edge는 source 속성값에 해당하는 노드에서 target 속성값에 해당하는 노드로 정보가 이동할 때의 노드간의 overlay 연결정보를 의미하며, 물리적인 링크가 아니라 임의의 링크들을 거쳐 데이터가 전달될때의 네트워크 속성이다. 사용하는 속성으로는 지연도, 지터링, 패킷 손실률이 있다.

3. `<plugin>`

`<plugin>` 태그는 각 가상 노드에서 돌아가는 어플리케이션을 위하여 필요한 바이너리 데이터를 의미한다. id 속성이 해당 바이너리 데이터를 사용할 때 필요한 고유 id값이며, path 정보가 해당 바이너리 데이터의 위치를 의미한다. 예시는 다음과 같다.

```<plugin id="NODE" path="libBLEEPlib-test-pow-node.so" />```

위 예시에서, 향후 plugin 속성값이 "NODE"인 가상 어플리케이션은 해당 shared object를 사용하여 구동되게 된다.

4. `<kill>`

`<kill>` 태그는 시뮬레이션의 종료 시간을 명시한다. time 속성 값만큼의 가상 시간에 대한 시뮬레이션을 진행한다.

5. `<node>`

`<node>` 태그는 시뮬레이션 위에서 사용되는 가상 기계의 추상화된 표현이며, id 속성을 입력함으로써 `<topology>` 태그 내에서 명시된 id가 동일한 노드의 네트워크 속성을 적용 할 수 있다.

가상 기계 내부의 각 어플리케이션은 `<node>` 태그 내부의 `<application>` 태그로 명시되며, `<application> 태그의 속성의 의미는 다음과 같다.
- plugin: 사용할 바이너리 실행프로그램
- time: 해당 프로그램을 시작할 시간(가상시간)
- arguments: 해당 프로그램을 시작할 때의 입력 파라미터

shadow를 구동하기 위하여 필요한 xml 데이터 예시는 다음과 같다.
```
<shadow>
  <!-- our network -->
  <topology>
    <![CDATA[<?xml version="1.0" encoding="utf-8"?><graphml xmlns="http://graphml.graphdrawing.org/xmlns" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://graphml.graphdrawing.org/xmlns http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd">
             <key attr.name="packetloss" attr.type="double" for="edge" id="d9" />
             <key attr.name="jitter" attr.type="double" for="edge" id="d8" />
             <key attr.name="latency" attr.type="double" for="edge" id="d7" />
             <key attr.name="asn" attr.type="int" for="node" id="d6" />
             <key attr.name="type" attr.type="string" for="node" id="d5" />
             <key attr.name="bandwidthup" attr.type="int" for="node" id="d4" />
             <key attr.name="bandwidthdown" attr.type="int" for="node" id="d3" />
             <key attr.name="geocode" attr.type="string" for="node" id="d2" />
             <key attr.name="ip" attr.type="string" for="node" id="d1" />
             <key attr.name="packetloss" attr.type="double" for="node" id="d0" />
             <graph edgedefault="undirected">
               <node id="poi-1">
                 <data key="d0">0.0</data>
                 <data key="d1">0.0.0.0</data>
                 <data key="d2">US</data>
                 <data key="d3">10000020000040</data>
                 <data key="d4">10000240</data>
                 <data key="d5">net</data>
               </node>
               <edge source="poi-1" target="poi-1">
                 <data key="d7">50.0</data>
                 <data key="d8">0.0</data>
                 <data key="d9">0.0</data>
               </edge>
             </graph>
             </graphml>]]>
  </topology>

  <!-- the plug-ins we will be using -->
  <plugin id="NODE" path="libBLEEPlib-test-pow-node.so" />


  <!-- the length of our experiment in seconds  1404101800-->
  <kill time="300" />
  <!-- our nodes -->

  <node id="pownode0">
    <application plugin="NODE" time="5" arguments="-id=pownode0 -txgenstartat=0 -txgeninterval=4" />
  </node>

  <node id="pownode1">
    <application plugin="NODE" time="6" arguments="-id=pownode1 -txgenstartat=10000000 -txgeninterval=10000000 -connect=pownode0" />
  </node>


</shadow>
```

# 서브모듈 관리
BLEEP은 Git 저장소를 이용해 소스코드를 관리하며 아래와 같은 서브모듈을 포함하고 있다. 서브모듈에 대한 정보는 루트 디렉터리의 `.gitmodules`에 정의 되어 있다. 
```
[submodule "shadow"]
        path = shadow
        url = https://github.com/kaistshadow/shadow
[submodule "external/bitcoin/0.19.1dev"]
        path = external/bitcoin/0.19.1dev
        url = https://github.com/kaistshadow/bitcoin.git
[submodule "external/monero/0.17.0.0dev"]
        path = external/monero/0.17.0.0dev
        url = https://github.com/kaistshadow/monero
```

# Git Action을 이용한 CI(Continuous Integration)
BLEEP을 이용한 개발 및 프로그램 품질 관리를 위해 Git Action을 사용하고 있다. 
- 예시) 비트코인 빌드 및 관련 테스트케이스에 대한 동작 검증
```
jobs:
  build:
    runs-on: ubuntu-18.04
    steps:
    - uses: actions/checkout@v2  
    - name: install pre-requisite
      run: |
        for apt_file in `grep -lr microsoft /etc/apt/sources.list.d/`; do sudo rm $apt_file; done
        sudo apt install python
        sudo apt-get update
    - name: install BLEEP
      run: python setup.py --bitcoin
    - name: test BLEEP using script
      run: |
        export PATH=${PATH}:${GITHUB_WORKSPACE}/Install/bin
        export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${GITHUB_WORKSPACE}/Install
        python setup.py --test
```
