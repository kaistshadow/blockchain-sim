## Debugging
### 1. Build with debugging symbol
 디버깅을 위해서는 빌드 시에 심볼을 포함하도록 설정을 해야한다. BLEEP 프로젝트는 이를 위해 `CMAKE_BUILD_TYPE`을 이용한다. `CMAKE_BUILD_TYPE`이 `Debug`로 세팅되어있으면, BLEEP의 CMake script들은 모든 컴포넌트들을 컴파일할 때 debug 심볼을 포함시키게 된다.
 따라서 CLion에서 CMake 설정을 하면 되는데, Setting(Preference)->Build, Execution, Deployment->CMake에서 프로필을 설정할 수 있다.
 
 ![CLion CMake 설정 예시](https://github.com/kaistshadow/blockchain-sim/blob/feature/191/documentation/doc/images/Developerguide-clion-cmake.png)

 위의 예시에서 중요한 부분은 CMake options 부분이다. `-DCMAKE_BUILD_TYPE=Debug`가 자동으로 세팅되어있는 것을 확인할 수 있다.

참고로, BLEEP 프로젝트에서 사용하는 추가적인 주요 CMake option들은 아래와 같다. 
| CMAKE option  |     value     |   default value | description    |     
| ------------- | ------------- |------------------|--------------|
| TEST_OPT      | ON  or OFF  |   OFF | regression test를 같이 빌드 |
| ALL_OPT  | ON or OFF  | OFF | 모든 블록체인을 같이 빌드  |
| BITCOIN_OPT | ON or OFF | OFF | 비트코인 블록체인만 빌드 (MONERO_OPT와 같이 사용 불가) |
| MONERO_OPT | ON or OFF | OFF | 모네로 블록체인만 빌드  (BITCOIN_OPT와 같이 사용 불가) |
| GIT_OPT  | ON or OFF  | OFF | git action에서 돌릴 일부 블록체인만 빌드(현재는 BITCOIN_OPT와 동일)|
||||(참고: ALL_OPT, BITCOIN_OPT, MONERO_OPT, GIT_OPT 가 모두 OFF일 때에는 비트코인만 빌드가 된다. 즉 BITCOIN_OPT=ON과 동일)|

 참고로 [remote development](https://www.jetbrains.com/help/clion/remote-projects-support.html#deployment-entry)를 사용할 때에는 Toolchain 항목에서 remote 서버의 toolchain을 선택해야한다. 

### 2. Run/Debug plugin
플러그인을 디버깅하기 위해서는 우선 기본적으로 Install 명령을 통해 Shadow를 설치해야한다.
CLion에서 Build->Install 명령을 사용하면 된다. 

Install이 완료되었다면 Run/Debug configuration을 추가해야한다. Run-> Edit Configurations 메뉴에서 아래와 같이 새로운 `CMake Application` 실행 설정을 추가한다. 

 ![CLion Run/Debug 설정 예시](https://github.com/kaistshadow/blockchain-sim/blob/feature/191/documentation/doc/images/Developerguide-clion-runconf.png)

Program arguments로 $FileName$을 세팅하고, Working directory에는 $FileDir$를 세팅해준다. 이렇게 세팅을 하면, CLion에서 실행/디버깅을 하고자 하는 shadow 설정파일 (예: `BLEEPeval/sybiltest-app/bitcoin/test-BitcoinP2P.xml`)을 열고 실행/디버깅을 동작시켰을 때, 이는 해당 shadow 설정파일이 있는 디렉토리 (예: `BLEEPeval/sybiltest-app/bitcoin/`)에서 `shadow test-BitcoinP2P.xml`을 실행/디버깅하는 작업과 동일하게 된다. 

### 3. Load plugin's debug symbols 

이제 원하는 설정에 대한 emulation을 동작시켜 디버깅을 진행할 수 있다. 
위의 단계에서 세팅한 CMake 설정 및 Run/Debug 설정을 선택한 뒤, Run->Debug 'run-with-shadow'를 수행한다. 
이 때, 동작시키는 plugin의 코드를 디버깅하기 위해서는 plugin의 디버깅 심볼을 로드해야만 한다.
이에 대한 자세한 방법은 [Shadow Developer Guide](https://github.com/shadow/shadow/blob/main/docs/5-Developer-Guide.md#debugging-plugins-with-gdb)을 참고하자. 

간단하게 말하자면, Shadow simulator가 각각의 plugin을 로드한 직후 (`shd-process.c`에서 `_process_loadPlugin`함수가 수행 완료된 직후) GDB 창에서 `p vdl_linkmap_abi_update()` 를 실행하면 로드된 plugin의 디버깅 심볼이 모두 로드되고, 따라서 plugin 코드 영역에 breakpoint를 거는 등의 디버깅이 가능해진다.