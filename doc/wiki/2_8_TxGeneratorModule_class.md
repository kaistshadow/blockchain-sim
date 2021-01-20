_TxGeneratorModule_ 은 random한 transaction을 생성하기 위한 API를 제공하고 있다. random한 transaction을 생성하는 API는 블록체인 노드를 구현하고 테스트할 때, 유용하게 사용할 수 있다. 즉, 블록체인 노드로 네트워크를 구성한 이후, random한 transaction을 생성하여 블록체인 노드에게 전송하는 역할을 하는 어플리케이션을 _TxGeneratorModule_ 과 _BasicNetworkModule_ 의 API들을 이용해 쉽게 구현할 수 있다. 자세한 구현 및 동작에 대해서는 [tutorial](https://github.com/kaistshadow/blockchain-sim/wiki/1.-Tutorial-:-Developing-a-simple-network-application-using-BLEEP)을 참고하도록 하자

## API for random transaction generation
### _AsyncGenerateRandomTransaction_ API
```
void AsyncGenerateRandomTransaction(double time);
```
**time**: random transaction을 몇초 후에 생성할 것인지를 설정

이 API가 호출되면, 인자로 주어진 _time_ 이후에 random한 transaction이 생성된다. API는 이를 기다리지 않고 곧바로 return되지만, _time_ 이후에 transaction이 생성되면 asynchronous event인 _CompleteAsyncGenerateRandomTransaction_ 이 발생하게 된다. 따라서 유저는 이 API를 호출한 뒤에, _Wait_ API를 호출함으로써 asynchronous event가 발생할때까지 기다리면 된다.

## List of asynchronous event 

### _CompleteAsyncGenerateRandomTransaction_ event
_AsyncGenerateRandomTransaction_ API에 의해 발생하는 event로써, random한 transaction이 생성되었을 때 이 event가 발생한다. 
이 이벤트는 아래와 같은 event data를 포함하고 있다.
```
boost::shared_ptr<Transaction> _generatedTx;
```
**_generatedTx** : 생성된 transaction에 대한 포인터 

