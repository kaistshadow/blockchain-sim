
`Shadow` 시뮬레이터를 이용한 대규모 네트워크 시뮬레이션 시, BLEEP은 여러 가상노드에서 사용되지만 가상 노드간 독립성으로 인하여 다른 메모리 영역에 할당되는 데이터에 대하여 메모리 영역 공유를 지원한다.
메모리 영역 공유 기법은 BLEEP에서 수정한 `Shadow` 시뮬레이터와 `Shadow`와 에뮬레이션 된 가상 노드사이의 인터페이스 함수를 기반으로 동작한다.
메모리 영역 공유 기법을 적용하기 위해서 메모리 영역 공유 기법 인터페이스에 맞춘 타겟 데이터 클래스의 함수 추가 및 에뮬레이션 대상 코드의 일부 수정이 필요하다.

# Prerequisites
현재 메모리 영역 공유 기법을 적용 가능한 데이터의 조건은 다음과 같다.
- 에뮬레이션 대상 코드는 C++ 기반으로 작성되어야 하며, 임의의 공유할 데이터는 클래스 타입이어야 함
- 공유할 데이터는 에뮬레이션 대상 코드에서 shared_ptr 기법을 이용하여 처리되어야 함(std::shared_ptr 또는 boost::shared_ptr 사용)
- 임의의 공유할 데이터 클래스 X의 필드 함수로 std::size_t hash() 함수와 bool operator==(const X& other) 함수가 구현되어있어야 함
  - hash 함수는 std::size_t의 타입을 가지는 고유 해시 데이터를 생성하는 함수
  - operator== 함수는 동일 클래스의 다른 데이터와 동일함을 비교하기 위한 함수
- 공유할 데이터는 공유 시점 이후 변경되지 않아야 함(공유 이후 데이터의 변경이 발생하는 경우, 예상하지 못한 결과가 발생 할 수 있음)
- 에뮬레이션 대상 코드에 인터페이스 헤더 추가 및 인터페이스 라이브러리 링크
  - 메모리 영역 공유 기법 인터페이스 헤더 `shadow_memshare_interface.h`는 `interfaces/shadow_memshare_interface`에 존재함
  - 메모리 영역 공유 기법 인터페이스 라이브러리 `libSHADOW_MEMSHARE_INTERFACE.so`는 `Shadow` 설치시 `Install/lib`에 함께 설치되며, 에뮬레이션 대상 코드 빌드시 해당 라이브러리를 함께 링크하여 빌드해야 함

## Example
다음 클래스 A에 메모리 영역 공유 기법을 적용하려고 한다.
```
class A {
  int x;
public:
  A() { x = 1; }
};
```
해당 클래스로 선언된 변수는 에뮬레이션 대상 코드 내에서 다음과 같이 shared_ptr 기법을 이용하여 관리된다고 가정한다.
```
int fn1() {
  ...
  std::shared_ptr sptr = std::make_shared<A>();
  ...
}
```
메모리 영역 공유 기법을 적용하기 위하여, std::size_t hash() 함수와 bool operator==(const A& other) 함수가 없는 경우, 의미에 맞게 추가 구현하여 다음과 같이 A를 수정한다.
```
class A {
  int x;
public:
  A() { x = 1; }
  std::size_t hash() {
    return std::hash<int>()(x);
  }
  bool operator==(const A& other) {
    return x == other.x;
  }
};
```
`cmake`이용시, `CMakeLists.txt`파일에서 빌드 타겟 T에 대하여 메모리 영역 공유 기법 라이브러리 링크 및 헤더 디렉토리를 추가한다.
```
...
add_library(T ...)
target_link_libraries(T SHADOW_MEMSHARE_INTERFACE)
include_directories(${CMAKE_ROOT_SOURCE_DIR}/interfaces/shadow_memshare_interface)
...
```

# Memory Sharing API
공유 가능한 임의의 클래스 X에 대한 에뮬레이션 대상 코드에서 사용되는 메모리 공유 기법 API는 다음과 같다.
- `void memshare::try_share(shared_ptr<X> sptr)`: `sptr`을 라이브러리상의 해시테이블에 등록함. 만약 해당 `shared_ptr`가 참조하는 데이터와 동일한 값을 가지는 데이터가 존재시 어떠한 효과도 없음.
- `shared_ptr<X> memshare::lookup(shared_ptr<X> sptr)`: `sptr`이 참조하는 데이터와 동일한 값을 가지는 데이터를 라이브러리상의 해시테이블에서 검색하여 찾은 `shared_ptr`를 반환함. 만약 동일한 데이터가 없다면 `sptr`을 그대로 반환.
  - `shared_ptr<X>` 타입의 임의의 변수 `a`에 대하여, `a = memshare::lookup(a)`와 같이 사용시 `shared_ptr` 매커니즘에 의하여 다른 메모리 영역에 할당된 동일한 데이터를 자동적으로 제거할 수 있음.

## Example
Prerequisites의 예시에서 명시된 `fn1`에서 A에 대한 공유를 수행하기 위하여 다음과 같이 코드를 추가 할 수 있다.
```
...
#include "shadow_memshare_interface.h"  // 인터페이스 함수 사용을 위한 헤더 추가
...
int fn1() {
  ...
  std::shared_ptr sptr = std::make_shared<A>();
  ...
  memshare::try_share(sptr);
  sptr = memshare::lookup(sptr);
  ...
}
```
위 코드와 같이 `memshare::try_share`과 `memshare::lookup` 함수를 연속적으로 사용함으로써 공유 해시테이블에 동일 데이터가 없다면 등록하고 동일 데이터가 존재한다면 기존 변수에서 참조하는 데이터의 메모리 할당을 해제하고 해시테이블의 데이터를 사용하도록 코드를 작성 할 수 있다.

# Memory Sharing Example
메모리 공유 기법의 성능 확인을 위하여 BLEEPlib의 PoW 블록체인 추상화 에뮬레이션에 메모리 공유 기법을 적용하였다.
이를 위하여 다음과 같이 수정하였다.
- 인터페이스 헤더 디렉토리 연결 및 인터페이스 라이브러리 링크
  - `BLEEPlib/CMakeLists.txt`에 `target_link_libraries(BLEEP SHADOW_MEMSHARE_INTERFACE)`, `include_directories(${CMAKE_ROOT_SOURCE_DIR}/interfaces/shadow_memshare_interface)`추가
- SimpleTransaction 클래스 및 PoWBlock 클래스의 함수 추가
  - `BLEEPlib/src/BL3_protocol/POWBlock.h`의 `POWBlock` 클래스에 public 함수 hash, operator== 추가
    ```
    std::size_t hash() {
      return (std::hash<std::string>()(block_hash.str()));
    }
    bool operator==(const POWBlock& other) {
      return nonce == other.nonce
      && tx_hash == other.tx_hash
      && prev_block_hash == other.prev_block_hash
      && timestamp == other.timestamp
      && difficulty == other.difficulty;
    }
    ```
  - `BLEEPlib/src/BL3_protocol/Transaction.h`의 `SimpleTransaction` 클래스 public 함수 hash, operator== 추가
    ```
    std::size_t hash() {
      return ((std::hash<int>()(sender_id)
      ^ (std::hash<int>()(receiver_id) << 1)) >> 1)
      ^ (std::hash<float>()(amount) << 1);
    }
    bool operator==(const SimpleTransaction& other) {
      return sender_id == other.sender_id
      && receiver_id == other.receiver_id
      && amount == other.amount;
    }
    ```
- SimpleTransaction 및 PoWBlock 데이터가 변경되지 않는 시점에서의 공유 매커니즘 추가
  - `BLEEPlib/src/BL3_protocol/BlockTree.cpp`의 genesis 트랜잭션, genesis 블록의 등록시도, 공유(`BlockTree<T>::BlockTree()` 함수)
    ```
    ...
    std::shared_ptr<SimpleTransaction> genesis_tx = std::make_shared<SimpleTransaction>(0, 0, 0);
    memshare::try_share(genesis_tx);
    genesis_tx = memshare::lookup(genesis_tx);
    genesis_tx_list.push_back(genesis_tx);

    std::shared_ptr<T> genesisblk = std::make_shared<T>("", genesis_tx_list);
    genesisblk->SetGenesisBlock();
    memshare::try_share(genesisblk);
    genesisblk = memshare::lookup(genesisblk);
    ...
    ```
  - `BLEEPlib/src/BL3_protocol/ProtocolLayerEx1.h`의 록시도션 공유(`_txgentimerCallback(ev::timer &w, int revents)` 함수)
    ```
    ...
    std::shared_ptr<SimpleTransaction> tx = std::make_shared<SimpleTransaction>(sender_id, receiver_id, amount);
    memshare::try_share(tx);
    tx = memshare::lookup(tx);
    ...
    ```
  - `BLEEPlib/src/BL3_protocol/ProtocolLayerPoW.cpp`의 블록 데이터 수신 후 공유(`_RecvPOWBlockBlkHandler(std::shared_ptr<Message> msg)` 함수)
    ```
    ...
    std::shared_ptr<POWBlock> blkptr = getdata->GetBlock();
    blkptr = memshare::lookup(blkptr);
    ...
    ```
  - `BLEEPlib/src/BL3_protocol/ProtocolLayerPoW.cpp`의 블록 생성 후 공유(`BL_ProtocolLayerPoW::SwitchAsyncEventHandler(AsyncEvent& event)` 함수)
    ```
    ...
    std::cout << "blockhash:" << libBLEEP::UINT256_t((const unsigned char*)minedBlk->GetBlockHash().str().c_str(), 32) << "\n";
    
    memshare::try_share(minedBlk);
    minedBlk = memshare::lookup(minedBlk);
    
    // append block to ledger
    ...
    ```
  - `BLEEPlib/src/BL3_protocol/ProtocolLayerPoW.h`의 트랜잭션 생성 후 공유(`BL_ProtocolLayerPoW::SwitchAsyncEventHandler(AsyncEvent& event)` 함수)
    ```
    ...
    std::shared_ptr<SimpleTransaction> tx = std::make_shared<SimpleTransaction>(sender_id, receiver_id, amount);
    memshare::try_share(tx);
    tx = memshare::lookup(tx);
    ...
    ```
  - `BLEEPlib/src/BL3_protocol/TxGossipProtocol.cpp`의 트랜잭션 수신 후 공유(`TxGossipProtocol::RecvTxsHandler(std::shared_ptr<Message> msg)` 함수)
    ```
    ...
    for (auto tx : txs) {
      std::cout << "receive tx:" << tx << "\n";
      
      // Add to txpool
      tx = memshare::lookup(tx);
      _txPool->AddTx(tx);
    }
    ...
    ```
- `memshare` 인터페이스를 사용하는 모든 코드에 `#include "shadow_memshare_interface.h"` 추가

성능 실험 환경을 `Bitcoin`의 평균 블록 크기 및 트랜잭션 크기, 블록 생성주기와 맞추도록 하기 위하여 추가로 다음을 수정하였다.
- `SimpleTransaction` 클래스 필드 변수로 `char dummy[200]` 추가(트랜잭션의 데이터 크기 조정을 위하여 200byte 추가)
- `POWBlock`의 트랜잭션 개수를 4000개 가량으로 설정(1MB 가량의 블록데이터로 가정)
- 전체 네트워크의 블록 생성 평균 시간을 600초 가량으로 설정

전체 네트워크의 설정을 다음과 같이 설정하였다.
- 전체 네트워크는 1000개의 BLEEPlib PoW 노드로 이루어진 트리형태의 네트워크로 구성
- 전체 노드들이 PoW 블록 채굴의 추상화된 알고리즘을 수행
- 한 블록당 4000개 가량의 트랜잭션이 지연없이 들어가도록 충분히 많은 트랜잭션을 하나의 노드가 생성하여 연결된 피어 노드로 전파
