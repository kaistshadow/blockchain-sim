_Block_ class는 블록체인에서의 데이터 단위인 block 을 구현한 class이다. 기본적인 생성자, 연산자 오버로딩와 함께 직렬화/역직렬화(boost) 기능을 제공하고 있고, _GetTransactions_, _GetId_ 라는 2개의 API를 제공하고 있다.


### _GetTransactions_ API
```c++
const std::list<boost::shared_ptr<Transaction> >& GetTransactions() const
```
**return value**: 현재 _Block_ 이 저장하고 있는 transaction들에 대한 포인터 list 를 return

_Block_ 에 포함되어있는 모든 transaction 에 대한 포인터 list 를 리턴한다.

### _GetId_ API
```c++
std::string GetId() const
```
**return value**: _Block_ 의 id (string)

_Block_ 의 id 값을 return 함. 현재 _Block_ class 에서는 _Block_ 을 생성할 때 생성자를 통해서만 id값을 세팅할 수 있다. 