_LedgerManager_ class는 blockchain을 관리하기 위한 class 로써, blockchain을 구성하고 있는 block들을 저장 및 관리할 수 있도록 해주는 class이다. _LedgerManager_ 는 기본 _Block_ class 가 리스트 형태로 블록체인을 구성하도록 구현되었다. blockchain 자체를 저장하고 불러오기 위한 _SetLedger_, _GetLedger_, blockchain 에 block을 추가하거나 마지막으로 추가된 블록을 가져오기 위한 _AppendBlock_, _GetLastBlock_, 블록체인에 다음에 추가될 block의 index 를 알려주는 _GetNextBlockIdx_, 그리고 file 형태로 blockchain을 저장하거나 불러올 수 있는 _LoadLedgerFromFile_, _SaveLedgerToFile_ 까지 총 7개의 API를 제공하고 있다.


### _SetLedger_ API
```c++
void SetLedger(std::list<Block> blocks)
```
**blocks** : blockchain 을 구성하는 block list

block list 를 인자로 받아서, 새롭게 blockchain 을 설정하게 된다. 만일 기존에 다른 블록체인을 저장하고 있었다면, 덮여쓰이기 때문에 기존 블록체인은 사라진다.

### _GetLedger_ API
```c++
std::list<Block> &GetLedger() 
```
**return value**: _LedgerManager_ 가 저장하고 있는 blockchain 에 대한 reference 변수를 return함

_LedgerManager_ 가 현재 저장(및 관리)하고 있는 블록체인에 대한 reference 변수를 return 한다. 따라서 넘겨받은 블록체인에 대한 reference 변수를 통해 직접 블록체인을 접근하거나 조작할 수 있다. (개발자 노트: 이처럼 넘겨받은 변수를 통해 블록체인을 직접 변경하는 것은 사실 바람직하지 않은 모델이다. 어떻게 사용자가 블록체인을 컨트롤할지 전혀 예측할 수 없기 때문이다. 현재의 _LedgerManager_ 를 개선하여, 향후에는 오직 _LedgerManager_ 를 통해 블록체인을 변경할 수 있도록 더 strict하게 encapsulation 해야한다. )

### _GetLastBlock_ API
```c++
Block* GetLastBlock()
```
**return value** : blockchain 을 구성하는 마지막 block 에 대한 포인터

blockchain 을 구성하는 마지막 block 에 대한 포인터를 return 한다. 해당 포인터는 실제 블록체인에 저장된 block을 가르키고 있으므로 수정할 경우, blockchain 을 직접 수정하는 효과를 보인다. 또한, 포인터가 _LedgerManager_ 가 관리하는 멤버변수를 직접 가르키고 있으므로, free 하면 안된다.

### _AppendBlock_ API
```c++
void AppendBlock(std::shared_ptr<Block> blk)
```
**blk** : 추가하려는 block에 대한 스마트포인터

blockchain 에 block을 새로 추가하는 API 이다. 스마트포인터를 받아서 똑같은 block을 copy constructor를 통해 생성한 뒤에 blockchain 에 추가하게 된다. 따라서 스마트포인터가 blockchain 을 구성하는 block을 직접 가르키지는 않게 된다. 

### _GetNextBlockIdx_ API
```c++
unsigned long GetNextBlockIdx()
```
**return value** : 블록체인에 추가될 다음 block이 가져야 하는 index 값

블록체인에 추가될 다음 block이 가져야 하는 index를 return하는 API이다. 여기서 block 의 index 는 _LedgerManager_ class 가 정의하는 값으로, 특별히 block 마다 관리되거나 할당되어 있지는 않다. 다음에 추가될 block index 를 계산하는 방법은 매우 단순하다. 아무것도 없을 때, 0을 return하며, block이 추가될때마다 1씩 증가한다. 즉, 결국엔 현재 가지고 있는 blockchain의 block 갯수를 return 하는 것과 마찬가지이다. 

### _SaveLedgerToFile_ API
```c++
void SaveLedgerToFile()
```

블록체인을 파일 형태로 저장하도록 하는 API이다. 어느 파일에서 저장할지는 _LedgerManager_ class 를 생성할 때에 생성자 argument로 지정할 수 있다. 나중에, _LoadLedgerFromFile_ API를 통해 다시 메모리상의 블록체인으로 읽어올 수 있다.

### _LoadLedgerFromFile_ API
```c++
void LoadLedgerFromFile()
```

블록체인을 파일로부터 읽어오도록 하는 API이다. 어느 파일에서 읽어올지는 _LedgerManager_ class 를 생성할 때에 생성자 argument로 지정할 수 있다. 또한, _SaveLedgerToFile_ 을 통해 저장한 파일만 정상적으로 읽어올 수 있다. 
