BLEEP needs a Ubuntu Linux. 
We confirmed that BLEEP works well in Ubuntu 16.04 LTS and Ubuntu 18.04 LTS.

# Installing dependencies

#### Required:
  + gcc, gcc-c++
  + cmake (version >= 3.2) (*)
  + make
  + boost
  + libev
  + glib (version >= 2.32.0) (*)
  + igraph (version >= 0.5.4)
  + xz-utils
  + glibc debuginfo
  + python 2, pyelftools (python module)
  + rustc (for regression tests)

(*) on Ubuntu 14.04, default installed version of cmake is 3.0.  
Thus, you probably need to manually build cmake or use PPA; 
see https://askubuntu.com/a/610352

(*) on Ubuntu 14.04, you probably need to manually build glib; 
Follow the instructions.
```bash
wget http://ftp.gnome.org/pub/gnome/sources/glib/2.42/glib-2.42.1.tar.xz
tar xaf glib-2.42.1.tar.xz
cd glib-2.42.1; ./configure --prefix=/home/${USER}/.shadow; make; make install
```

Ubuntu renamed libigraph0 to libigraph0v5 sometime between 14.04 and 16.04. Therefore when using a version of Ubuntu older than 16.04, you should replace `libigraph0v5` with `libigraph0`.

```bash
sudo apt-get install -y gcc g++ cmake make libboost-all-dev libev-dev libglib2.0-0 libglib2.0-dev libigraph0v5 libigraph0-dev cmake make xz-utils libc6-dbg libffi-dev python python-pyelftools rustc
```

# Installing BLEEP 

```bash
git clone https://github.com/kaistshadow/blockchain-sim
cd blockchain-sim
git submodule update --init
mkdir build
cd build
cmake ..
make
make install
cd ..
```

When you executes `make install`, BLEEP will be installed in sub-directory `Install`.

현재 blockchain-sim 디렉토리에 있다면 아래와 같이 설치된 emulator(shadow)의 path를 export한다.

```
echo "export PATH=$PATH:${PWD}/Install/bin" >> /home/${USER}/.bashrc
source /home/${USER}/.bashrc
```
설치가 잘 진행되었다면, 아래의 명령어를 통해 설치된 emulator(shadow) version 을 확인할 수 있다.
```
shadow --version
```

# Testing BLEEP 

테스트를 위해 아래와 같이 LD_LIBRARY_PATH에 설치된 plugin 경로를 추가한다.
```
echo "export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${PWD}/Install/lib:${PWD}/Install/plugins" >> /home/${USER}/.bashrc
source /home/${USER}/.bashrc
```

아래와 같이 미리 설정된 BLEEP 테스트를 수행한다.
```
cd tests/regtest
shadow -d datadir test-simpleconnect/simple-connect.xml
```

visualization을 원할 경우 필요한 library들을 설치한뒤, 아래와 같이 수행한다.
```
python test.py test-simpleconnect/simple-connect.xml
```
