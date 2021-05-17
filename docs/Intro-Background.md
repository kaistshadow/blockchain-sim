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
shadow는 네트워크 동작을 시뮬레이션을 하기 위한 오픈소스 프로그램이다. shadow에서 대해는 Shadow 프로젝트에서 제공하는 튜토리얼을 보면 자세히 알 수 있다. shadow는 BLEEP install 과정에서 함께 설치된다.
- [Shadow 튜토리얼](https://github.com/shadow/shadow/blob/main/docs/2-Getting-Started-Tutorial.md)
## XML
XML파일에서 시뮬레이션할 네트워크 토폴로지와 플러그인을 정의한다. 정의된 xml파일은 shadow실행 시 argument파일로 입력이 된다. 시뮬레이션이 실행이 되면 xml에서 설정한 값에 따라 네트워크가 시뮬레이션이 된다. 
- [XML 파일 작성 방법](https://github.com/shadow/shadow/blob/main/docs/3.2-Network-Config.md)

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
BLEEP을 이용한 개발 및 프로그램 품질 관리를 위해 Git Action을 사용함. 
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


