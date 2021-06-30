# BLEEP Blockchain Library Proof of Stake
`BLEEPlib`은 시뮬레이션을 위한 합의 알고리즘 중 하나로 PoS를 제공한다. PoS 관련 처리는 `ProtocolLayerPoS.cpp`, `ProtocolLayerPoS.h` 파일을 중심으로 이뤄지며, 수행되는 내용은 다음과 같다.
- PoS 관련 메세지 핸들링: 수신된 Inventory 메세지, Block 요청 메세지, Transaction 요청 메세지, 블록 전송 메세지 등을 처리
- 타이머 함수 처리: 주기적인 트랜잭션 생성 및 PoS 슬롯 마다의 블록생성 리더 선정 로직 수행

# Message Handling Function Detail
외부로부터 수신한 Protocol layer 관련 메세지를 핸들링 하는 코드는 다음과 같다 (`ProtocolLayerPoS.cpp`).
```
void BL_ProtocolLayerPoS::RecvMsgHandler(std::shared_ptr<Message> msg) {
    MessageType msgType = msg->GetType();
    if (msgType == "TXGOSSIP-INV") {
        _txGossipProtocol.RecvInventoryHandler(msg);
    } else if (msgType == "TXGOSSIP-GETDATA") {
        _txGossipProtocol.RecvGetdataHandler(msg);
    } else if (msgType == "TXGOSSIP-TXS") {
        _txGossipProtocol.RecvTxsHandler(msg);
    } else if (msgType == "POSBLOCK-INV") {
        _RecvPOSBlockInvHandler(msg);
    } else if (msgType == "POSBLOCK-GETBLOCKS") {
        _RecvPOSBlockGetBlocksHandler(msg);
    } else if (msgType == "POSBLOCK-GETDATA") {
        _RecvPOSBlockGetDataHandler(msg);
    } else if (msgType == "POSBLOCK-BLK") {
        _RecvPOSBlockBlkHandler(msg);
    }
}
```
## Transaction Inventory Handling
외부로부터 트랜잭션 Inventory 메세지 `TXGOSSIP-INV`를 받는 경우 `RecvInventoryHandler` 함수가 수행된다. 해당 함수의 동작은 다음과 같다.
- 받은 트랜잭션 Inventory 메세지에 존재하는 트랜잭션 ID 중 자신의 트랜잭션 풀에 존재하지 않는 트랜잭션 ID에 대하여 트랜잭션 데이터를 요청(`TXGOSSIP-GETDATA`)하는 메세지 구성
- 구성된 트랜잭션 요청 메세지를 Inventory 메세지를 전달해 준 피어에게 피어 연결 계층 API를 이용하여 전송
## Transaction Request Handling
외부로부터 트랜잭션 데이터 요청 메세지 `TXGOSSIP-GETDATA`를 받는 경우 `RecvGetdataHandler` 함수가 수행된다. 해당 함수의 동작은 다음과 같다.
- 받은 트랜잭션 요청 메세지에 존재하는 트랜잭션 ID들에 매칭되는 트랜잭션 데이터가 자신의 트랜잭션 풀에 존재하는 경우 해당 트랜잭션들을 트랜잭션 풀에서 가져와 메세지를 구성
- 구성된 메세지(`TXGOSSIP-TXS`)를 트랜잭션 요청을 보낸 피어에게 피어 연결 계층 API를 이용하여 전송
## Transaction Data Response Handling
외부로부터 트랜잭션 데이터 메세지 `TXGOSSIP-TXS`를 받는 경우 `RecvTxsHandler` 함수가 수행된다. 해당 함수의 동작은 다음과 같다.
- 받은 메세지에 존재하는 트랜잭션 데이터들이 자신의 트랜잭션 풀에 존재하지 않는 경우, 트랜잭션 풀에 해당 트랜잭션 데이터들을 추가
- 받은 메세지에 존재하는 트랜잭션 데이터들의 트랜잭션 ID를 이용하여 트랜잭션 Inventory 메세지를 구성
- 트랜잭션 전파를 위하여 구성된 트랜잭션 Inventory 메세지를 자신의 이웃 피어들에게 전달
## Block Inventory Handling
## Block Request Handling
## Block Data Response Handling

# Timer Function Detail
BLEEP PoS node는 주기적으로 트랜잭션을 생성하며, 트랜잭션의 생성은 BL_ProtocolLayerPoS 인스턴스의 `_txgentimer` 타이머 오브젝트에 의하여 주기적으로 수행된다. 또한 PoS 로직에 따라 일정한 간격 (`Slot`)마다 블록을 생성할 리더를 결정하며, 리더 선택 로직은 BL_ProtocolLayerPoS 인스턴스의 `_slottimer` 타이머 오브젝트에 의하여 주기적으로 수행된다.
## Transaction Generation
## Slot Leader Selection
