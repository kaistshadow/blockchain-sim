_TxPool_ class는 transaction들을 저장하고 관리하기 위한 매니저 class이다. 기본적인 생성자, 연산자 오버로딩와 함께 직렬화/역직렬화(boost) 기능을 제공하고 있고, _GetPendingTxNum_, _GetTxs_, _RemoveTxs_, _AddTxs_, _AddTx_ 까지 총 다섯개의 API를 제공하고 있다.

### _GetPendingTxNum_ API
```c++
int GetPendingTxNum()
```
**return value**: 현재 _TxPool_ 이 저장하고 있는 transaction의 개수를 리턴

### _GetTxs_ API
```c++
std::list<boost::shared_ptr<Transaction> > GetTxs(int num);
```
**num** : 가져오고 싶은 transaction 의 갯수  
**return value**: _TxPool_ 이 저장하고 있는 transaction에 대한 포인터들의 list  

transaction 개수를 입력받아, 현재 _TxPool_ 더 많은 개수의 transaction을 저장하고 있을 경우, 해당 개수만큼의 transaction들에 대한 포인터를 리턴하는 함수. 이 때, transaction은 _TxPool_ 에서 삭제되거나 하지 않으며, pointer만 생성하여 리턴함. 
이때 pointer를 생성할 transaction이 선택되는 순서는 _TxPool_ 에 transaction이 추가된 순서와 동일함.

### _RemoveTxs_ API
```c++
void RemoveTxs(const std::list<boost::shared_ptr<Transaction> >& txs)
```
**txs** : transaction 포인터들의 list  

transaction 포인터 리스트를 받아서, 현재 _TxPool_에 저장되어 있는 transaction 중에 일치하는 모든 transaction 을 pool에서 삭제한다. 이때, _Transaction_ class가 제공하는 == 연산자를 이용한다. 즉 pointer 값으로 비교해서 삭제하는 것이 아니라, 실제 transaction 값을 비교해서 입력으로 받은 transaction과 값이 동일한 transaction들을 삭제해준다.

### _AddTx_ API
```c++
void AddTx(boost::shared_ptr<Transaction> tx)
```
**tx** : transaction 포인터

_TxPool_ 에 transaction 을 추가해준다. 현재 _TxPool_ class는 transaction 들의 pointer 를 관리하는 형식으로 구현되어 있다. 따라서 이 API는 스마트 포인터를 받아서 그대로 _TxPool_ 이 관리하는 transaction 포인터 pool 에 추가해주는 작업만 한다.

### _AddTxs_ API
```c++
void AddTxs(std::list<boost::shared_ptr<Transaction> > txs)
```
**txs** : transaction 포인터들의 list

_TxPool_ 에 transaction 들을 추가해준다. transaction 포인터 리스트를 받아서, _TxPool_ 에 해당 transaction 들을 추가해준다. 