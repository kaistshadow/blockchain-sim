BLEEP은 blockchain application 을 쉽게 디버깅하고 검증할 수 있도록 visualization 을 이용한 evaluation 기능을 지원하고 있다.

이 tutorial 에서는 사용자가 어떻게 BLEEP이 제공하는 evaluation 기능을 사용하여, 개발한 application을 디버깅 및 검증할 수 있는지 설명하도록 하겠다.

# Installing dependencies
우선 자신의 개발 환경이 ubuntu linux 환경인지를 확인해야한다. 
우리는 BLEEP을 통한 emulation이 Ubuntu 16.04 LTS.와 Ubuntu 18.04 LTS에서 제대로 동작하는 것을 확인하였다. 

그리고 BLEEP 저장소를 clone하지 않았다면 아래와 같이 다운받도록 한다.
```bash
git clone https://github.com/kaistshadow/blockchain-sim
cd blockchain-sim
```

#### Required dependencies:
  + python, python-pip
  + nodejs
  + nodejs modules
  + visjs

```bash
curl -sL https://deb.nodesource.com/setup_10.x | sudo -E bash -
sudo apt-get update
sudo apt-get install -y nodejs
sudo apt-get install -y python python-pip
cd BLEEPeval/web-gui;
npm install websocket finalhandler serve-static jsonpath @maxmind/geoip2-node
cd vis
npm install
npm run build
cd ../
```


# Using BLEEP evaluation framework (BLEEPeval) with example

#### Prerequisites:
+ 예제 어플리케이션 (shadow-plugin) 및 블록체인 시뮬레이터(shadow)의 compile 및 install (참고: [Installing BLEEP](https://github.com/kaistshadow/blockchain-sim/wiki/1.-Install-BLEEP#installing-bleep))


테스트를 위한 어플리케이션 플러그인과 블록체인 시뮬레이터가 설치되었다면, 
시뮬레이션 설정파일을 `visualize-events.py` 스크립트에게 넘겨주면 실험이 시작된다.
`visualize-event.py` 는 Shadow simulator가 사용하는 시뮬레이션 설정 파일을 그대로 사용한다.
이 예제에서는 위에서 설치한 어플리케이션들을 동작시키도록 미리 작성해놓은 설정파일을 ([`simple-connect.xml`](https://github.com/kaistshadow/blockchain-sim/blob/master/tests/regtest/test-simpleconnect/simple-connect.xml)) 사용할 것이다. 자세한 xml 설정 방법은 [shadow wiki](https://github.com/shadow/shadow/wiki/3.1-Shadow-Config)를 참고하도록 하자.

```bash
cd ../
python visualize-events.py ../tests/regtest/test-simpleconnect/simple-connect.xml
```

아래와 같은 주소로 웹브라우저를 통해 접속을 하면 설정파일을 동작시켰을 때 발생한 event의 list와 함께, 
이에 대한 visualization 이 진행된 것을 확인할 수 있다.
ipaddress는 위의 명령어를 실행한 머신의 ip 주소로 바꾸도록 한다.
```
http://ipaddress:1337/frontend.html
```


web에 접속한 뒤에, StartFromBeginning 버튼과, Next 버튼을 이용하면, 받아온 event 로그들을 step-by-step으로 시각화할 수 있다.
이를 통해, 구현한 어플리케이션이 실제로 어떻게 네트워크를 구성하며 어떤 메시지를 주고 받는지 쉽게 확인하고 디버깅할 수 있다.
`Event logs:`에 찍히는 event들은 `event가 발생한 시간,event 타입,event arguments`의 순서로 출력된다.
예를 들어 `35002000001,UnicastMessage,client2,bleep1,newTx`라는 이벤트는, 글로벌 시뮬레이션 시각으로 35.002000001초에 `UnicastMessage` 이벤트가 발생했고, 이 이벤트는 'client2 노드가 bleep1으로 newTx라는 메시지를 전송요청함'이라는 정보를 담고 있다. (위의 예제에서는 UnicastMessage 이벤트가 아닌 다른 이벤트가 발생하지만 비슷한 방식으로 로깅됨)

아래는 web 화면을 캡쳐한 모습이다.

[[https://github.com/kaistshadow/blockchain-sim/blob/master/doc/images/visualization-event.png]]


