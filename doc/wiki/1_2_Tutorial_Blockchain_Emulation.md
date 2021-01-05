BLEEP은 blockchain application 을 쉽게 동작시켜볼 수 있도록 네트워크 시뮬레이션을 지원하고 있다.

이 tutorial 에서는 사용자가 어떻게 BLEEP이 제공하는 네트워크 시뮬레이터를 설치하고, 이를 이용해 개발한 application을 테스트해볼 수 있는지 설명하도록 하겠다. 

# Installing dependencies
우선 자신의 개발 환경이 ubuntu linux 환경인지를 확인해야한다. 
우리는 BLEEP을 통한 emulation이 Ubuntu 16.04 LTS.와 Ubuntu 18.04 LTS 에서 제대로 동작하는 것을 확인하였다. 

#### Required:
  + gcc, gcc-c++
  + cmake (version >= 3.0)
  + make
  + glib (version >= 2.32.0) (*)
  + igraph (version >= 0.5.4)
  + xz-utils
  + glibc debuginfo

Ubuntu renamed libigraph0 to libigraph0v5 sometime between 14.04 and 16.04. Therefore when using a version of Ubuntu older than 16.04, you should replace `libigraph0v5` with `libigraph0`.

```bash
sudo apt-get install -y gcc g++ libglib2.0-0 libglib2.0-dev libigraph0v5 libigraph0-dev cmake make xz-utils libc6-dbg libffi-dev
```

(*) on Ubuntu 14.04, you probably need to manually build glib; 
Follow the instructions.
```bash
wget http://ftp.gnome.org/pub/gnome/sources/glib/2.42/glib-2.42.1.tar.xz
tar xaf glib-2.42.1.tar.xz
cd glib-2.42.1; ./configure --prefix=/home/${USER}/.shadow; make; make install
```



# Compile BLEEP emulator (shadow)

우선, BLEEP 저장소를 clone하지 않았다면 아래와 같이 다운받도록 한다.
```bash
git clone https://github.com/kaistshadow/blockchain-sim
cd blockchain-sim
```

BLEEP은 blockchain의 simulation을 위해 [shadow open-source simulator](https://github.com/shadow/shadow) 의 [수정된 버젼](https://github.com/kaistshadow/shadow)을 submodule로 포함하고 있다. 

이 submodule을 컴파일하기 위해서는 아래와 같은 명령어를 사용한다.

```bash
git submodule init
git submodule update
mkdir shadow/build
cd shadow/build
cmake ..
make
```

# Install BLEEP emulator (shadow)

```bash
make install
```

`make install`을 수행하면 루트 프로젝트 디렉토리 밑의 `Install` 디렉토리에 컴파일된 BLEEP emulator(shadow)가 설치된다.

설치된 emulator(shadow)의 path를 아래와 같이 export한다.
```bash
cd ../.. 
echo "export PATH=$PATH:${PWD}/Install/bin" >> /home/${USER}/.bashrc
source /home/${USER}/.bashrc
```

설치가 잘 진행되었다면, 아래의 명령어를 통해 shadow version 을 확인할 수 있다.
```
shadow --version
```
실행 결과는 대략적으로 아래와 같다
```
Shadow 2020-01-02 (built 2020-03-06) running GLib v2.56.4 and IGraph v0.7.1
For more information, visit https://shadow.github.io or https://github.com/shadow
```

# Running BLEEP emulator (shadow) with example

#### Prerequisite:
+ 어플리케이션 (shadow-plugin)의 compile 및 install (참고: [Compililing, linking example application](https://github.com/kaistshadow/blockchain-sim/wiki/1.1-Tutorial-:-Blockchain-application-development#compililing-linking-example-application))


어플리케이션 컴파일 및 설치가 완료되었다면, 
아래와 같이 실험을 위한 설정파일을 `shadow` 명령어에 넘겨주면 실험이 시작된다.
우리가 사용하는 Shadow simulator는 시뮬레이션을 설정하기 위해 xml파일을 사용한다.
우리는 xml을 통해 시뮬레이션에서 동작시킬 application들에 대한 세팅을 할 수 있다. 이 예제에서는 위에서 설치한 어플리케이션들을 동작시키도록 미리 작성해놓은 설정파일을 ([`rc1-eventloop.xml`](https://github.com/kaistshadow/blockchain-sim/blob/master/BLEEPeval/config-examples/rc1-eventloop.xml)) 사용할 것이다. 자세한 xml 설정 방법은 [shadow wiki](https://github.com/shadow/shadow/wiki/3.1-Shadow-Config)를 참고하도록 하자.
```
cd ./BLEEPeval
shadow -d rc1-eventloop-datadir config-examples/rc1-eventloop.xml
```
위의 명령어는 `-d` 옵션을 통해 shadow 시뮬레이션 결과를 저장할 위치를 `rc1-eventloop-datadir`로 설정한 것이다. 시뮬레이션동안 각 노드에서 stdout, stderr 으로 출력한 내용들은 `rc1-eventloop-datadir/hosts` 디렉토리에 log파일로 저장된다. 

명령어가 실행되면 shadow가 실행로그를 출력하게 되며, 이 중에 아래와 같은 메시지가 있을 경우, 실험이 무사히 진행된 것이다.
```
** Stopping Shadow, returning code 0 (success)
```
