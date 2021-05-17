이 페이지는 BLEEP을 설치하고 환경설정하는 방법에 대해 소개한다.<br>
※ 참고 : Ubuntu 18.04 LTS에서 BLEEP을 설치하는 것을 권장한다.  

# Dependencies
#### Required:
  - [Shadow](https://github.com/shadow/shadow/blob/main/docs/1.1-Shadow.md)  
    ```bash
    sudo apt-get install libc6-dbg     
    sudo apt-get install -y python python-pyelftools python-pip python-matplotlib python-numpy python-scipy python-networkx python-lxml
    sudo apt-get install -y git dstat git screen htop libffi-dev libev-dev
    sudo apt-get install -y gettext
    sudo apt-get install -y gcc g++ libglib2.0-0 libglib2.0-dev libigraph0v5 libigraph0-dev cmake make xz-utils
    ```
  - BLEEP
    ```bash
    sudo apt-get install -y libcurl4-openssl-dev
    sudo apt-get install -y libidn2-dev
    sudo apt-get install -y libzmq3-dev
    sudo apt-get install -y libboost-all-dev libjsoncpp-dev 
    ``` 
  - BITCOIN(v0.19.1)
    ```bash
    sudo apt-get install -y autoconf libtool libevent-dev libdb++-dev
    sudo apt-get install -y libssl-dev   
    ```
  - Monero(v0.17.0) 
    ```bash
    sudo apt install libminiupnpc-dev
    ```

# BLEEP 설치 및 환경설정

BLEEP은 모든 설치 과정을 자동화한 파이썬 스크립트를 이용하는 방법과 매뉴얼하게 설치하는 방법이 있으며
사용자 편의에 맞게 CMAKE에 `DCMAKE_BUILD_TYPE`옵션(릴리즈 or 디버그)을 적용하여 빌드 가능하다. 

## Python Scrpits    
BLEEP은 빠른 설치를 위해 파이썬 스크립트를 제공한다.  
아래의 명령어로 저장소를 다운로드받고 필요한 모든 컴포넌트들을 쉽게 설치할 수 있다.

```bash
git clone https://github.com/kaistshadow/blockchain-sim
cd blockchain-sim
python setup.py --all
source ~/.bashrc
```

또한 아래와 같이 테스트를 할 수 있다.
```bash
python setup.py --test
```

## Manual Build
BLEEP을 매뉴얼하게 설치하기 위한 절차는 다음과 같다. 
#### 1. Download BLEEP Repository
```bash
git clone https://github.com/kaistshadow/blockchain-sim
cd blockchain-sim
git submodule update --init
```
#### 2. Install BLEEP Dependencies
앞서 설명한 `Dependencies` 참고

#### 3. Build BLEEP Binaries
```bash
cd blockchain-sim
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j4
```
아래의 명령어로 설치한다. (프로젝트 루트 디렉토리의 Install 디렉토리로 필요한 파일들이 설치된다.)
```bash
make install
```

#### 4. Test BLEEP Binaries
제대로 컴파일 및 설치가 되었는지 확인하기 위해 테스트 파일들을 빌드하고 실행해 볼 수 있다.
```bash
cmake -DTEST_OPT=ON -DCMAKE_BUILD_TYPE=Release ..
make
make test
```

## Run BLEEP TestCase(e.g., EREBUS)
SybilAPI를 이용해 구현된 bitcoin에 대한 EREBUS 공격실험은 아래의 명령어를 통해 실행해볼 수 있다.
```bash
cd ../BLEEPeval/sybiltest-app/bitcoin
../../../Install/bin/shadow test-BitcoinP2P.xml
```

실험 결과는 shadow.data 폴더에 저장되는 노드들의 stdout 및 stderr 출력값을 통해 알 수 있다. 


## 시스템 설정 및 제한
리눅스 시스템에서 대규모의 노드(___1000노드 이상___)를 시뮬레이션을 하기 위해서는 시스템 설정을 변경해야 한다. 
- `root`계정에서 실행하는 경우
```
ulimit -n 65535
```
- `사용자`계정에서 실행하는 경우
```
sudo su
su {사용자 계정}
ulimit -n 65535
```



