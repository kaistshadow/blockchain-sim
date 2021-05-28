# The BLEEP
- BLEEP(BLockchain Emulation and Evaluation Platform)은 블록체인 어플리케이션과 기반이 되는 네트워크의 동작을 실제와 같이 시뮬레이션 및 에뮬레이션하여 동작을 검증하기 위한 테스팅 플랫폼이다.
- 블록체인 어플리케이션의 성능 및 취약성 분석을 위한 다양한 테스트 프레임워크를 제공한다. 
- 또한, 개발자에게는 블록체인 기반기술 개발을 위한 P2P 프로토콜 및 합의 알고리즘과 같은 라이브러리와 인터페이스를 제공한다.
- BLEEP은 오픈소스 플랫폼인 `Shadow`를 기반으로 개발되었으며, `Shadow`는 이산이벤트 시뮬레이터로 익명성 네트워크(The Onion Router)를 시뮬레이션하는 오픈소스 플랫폼이다. 

# What is core aims? 
* 실제 블록체인 어플리케이션 바이너리를 시뮬레이션 환경에서 동작 시킨다.(e.g., bitcoind)
* 다양한 블록체인 어플리케이션 개발에 활용할 수 있는 모듈형 라이브러리를 지원한다.(e.g., P2P Network, Consensus algorithm)
* 다양한 패턴의 트랜잭션 또는 블록을 벤치마킹하기 위한 일반화된 인터페이스를 제공한다.
* 블록체인 어플리케이션의 성능(e.g., TPS, Latency) 및 취약성 분석(e.g., Eclipse attack, EREBUS attack)을 위한 테스트 프레임워크를 제공한다. 
* BLEEP 실행 결과물을 시각화하여 블록 및 트랜잭션 생성, 전파와 같은 동작을 확인할 수 있다.
* 단일 머신에서 수천 개 이상의 노드 시뮬레이션이 가능한 고성능 테스트 환경을 제공한다. 

# How to run BLEEP?
[BLEEP document](docs/README.md)를 참고한다. <br>
※ Ubuntu 18.04 LTS 환경에서 설치 및 사용하는것을 권장 

# How to do BLEEP?
## Test frameworks
- Emulation <br>
	- 비트코인 다중 노드 에뮬레이션 <br>
		BLEEP은 비트코인의 다중 노드 에뮬레이션을 지원하기 위한 BLEEPemul폴더가 있다. BLEEPemul폴더안에는 채굴동작과 관련된 `one_node_setmine.cpp`, 트랜잭션을 생성하는 `transaction.cpp`파일과 이를 종합적으로 테스트하기 위한 `start_emulation.py` 스크립트 파일이 존재한다. 실험 구성은 testlibs폴더안에 있는 `test_modules.py`, `utils.py`, `xml_modules.py`, `test_result.py`를 수정하여 변경할 수 있다. BLEEP은 빌드가 완료되면 테스트 구성에 필요한 모든 파일이 빌드 경로에 자동으로 포함된다. 

- Evaluation <br>
	- Sybil Attack (e.g., monerod) <br>
		시빌 공격이란 한 개인이 다수의 노드를 구성해 네트워크를 장악하려는 보안 위협 중 하나이다. BLEEP은 시빌 공격을 에뮬레이션 하기위한 BLEEPeval/sybiltest-app 폴더가 있다. sybiltest-app 폴더안에는 ........
	- TPS 및 Latency 측정(e.g., bitcoind) <br>
		 TPS와 Latency는 네트워크 환경에 따른 어플리케이션의 성능을 측정하하기 위한 대표적인 성능 지표이다. BLEEP은 TPS와 Latency를 에뮬레이션 하기 위한 BLEEPeval/tpstest-app 폴더가 있다. tpstest-app 폴더안에는............


## What you have to do?
- 프로그램 동작 및 결과 확인과 관련된 자세한 내용은 [BLEEP document](docs/README.md)를 참고한다. 
- 먼저, BLEEP에서 어플리케이션을 에뮬레이션하기 위해 대상 바이너리를 공유라이브러리 형태로 변환 한다. 
	- BLEEP은 Shadow 시뮬레이터를 기반으로 개발되었으며, Shadow는 공유 라이브러리로 변환된 바이너리를 동적으로 로드하여 시뮬레이션한다.   
    - 공유라이브러리로 변환하는 방법은 컴파일 옵션에 직접 `-fPIC`, `shared` 옵션이 추가하는 방법이 있으며,
    - CMake를 이용할 경우 `add_library()`를 이용하거나 Shadow에서 제공하는 `add_shadow_plugin()`을 이용하면 된다. 
  
    ```
    예시)
    add_shadow_plugin("${name}" 
				${ARGN})
	target_link_libraries("${name}"
				PRIVATE
				${EXTRA_LIBRARIES})
	set_property(TARGET "${name}"
				PROPERTY
				LIBRARY_OUTPUT_DIRECTORY "${CMAKE_INTALL_PREFIX}")
    ```
    - BLEEP은 현재 비트코인(ver.0.19.1) 및 모네로(ver.0.17.0) 그리고 블록체인 어플리케이션의 동작을 돕는 다양한 공유 라이브러리를 제공하고 있다. 

- BLEEP에서 제공하는 테스트케이스를 이용한 실험 <br>
	실험을 하고자하는 테스트케이스 폴더로 이동해 해당 스크립트 코드를 실행한다. 
	- 1.Emulation 실험이 필요하다면 BLEEPemul 폴더로 이동해 사용자 매뉴얼을 따른다.
	- 2.Evaluation 실험이 필요하다면 BLEEPeval 폴더로 이동해 사용자 매뉴얼을 따른다. 		
	- 3.실험에서 제공하는 스크립트 코드를 이용해 Argument 값을 변경하며 에뮬레이션을 동작 시킨다. 	  
	- 4.실험 결과는 생성된 shadow.data 폴더에서 확인한다. 

- BLEEP에서 제공하는 테스트 프레임워크를 이용한 테스트케이스 제작 및 실험 <br>
	`1. CMakeLists.txt`, `2. 환경설정 파일`, `3. 테스트를 위한 스크립트 코드`를 사용자 요구에 맞게 제작한다. 
	- 1.실험을 위해 필요한 플러그인 코드(C/C++기반)를 작성한다. 		
		- BLEEP라이브러리를 이용할 경우 제공하는 라이브러리 및 인터페이스에 맞게 코드를 추가하여 구현한다. <br>
		※ 플러그인 : 에뮬레이션 대상이 되는 공유 라이브러리 형태의 어플리케이션		
	- 2.기 작성된 `CMakeLists.txt` 파일을 참고해 플러그인과 실험에 필요한 파일을 빌드 경로에 포함시킨다. 		
	- 3.XML 형식의 환경설정 파일(네트워크 토폴로지 및 플러그인 정의)을 작성한다. <br>
		※ Shadow 프로세스는 XML파일에 정의된 내용에 맞는 네트워크 및 노드의 시뮬레이션 환경을 구성한다. 
    - 4.환경설정에 정의된 Argument 값을 변경하여 에뮬레이션을 동작 시킨다. 
	- 5.실험 결과는 생성된 shadow.data 폴더에서 확인한다. 
	- 6.구현 완료된 테스트케이스는 BLEEP에서 제공하는 테스트케이스와 같이 스크립트코드를 이용해 자동화 한다. 


## How to contribute?
- **Questions and Bug Reports** <br>
[https://github.com/kaistshadow/blockchain-sim/issues](https://github.com/kaistshadow/blockchain-sim/issues/new/choose)

	> - **Branch Naming** <br>
	> BLEEP은 develop 브랜치를 이용해 개발을 진행하고 해당 버전 릴리즈가 완료되면 master에 병합하고 있다. 
	>	- release/x.x.x : develop 브랜치로부터 생성
	>	- feature/<issue_number>/<짧은 설명> : 완료 후 develop 브랜치에 병합
	>	- hotfix/<issue_number> : master 브랜치로부터 생성
	>	- issue/<issue_number> : develop, feature, release 브랜치로부터 생성

	> - **How to write a git commit message** <br>
	>	- 커밋 제목
	>		- 커밋 제목과 본문 사이에 한줄 띄울것
	>		- 커밋 제목은 50자 이내
	>		- 커밋 제목은 첫글자가 대문자로 시작
	>		- 커밋 제목 끝에 온점 붙이지 말 것
	>		- 커밋 제목은 명령형으로 쓸 것
	>			- e.g., Implement A to B, Rename A to B, Fix typo
	>	- 커밋 내용
	>		- 72자 이내로 다음 줄로 개행할 것
	>		- 무엇을 바꿨는지, 왜 바꿨는지 쓸 것


