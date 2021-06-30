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
## Transaction Request Handling
## Transaction Data Response Handling
## Block Inventory Handling
## Block Request Handling
## Block Data Response Handling

# Timer Function Detail
BLEEP PoS node는 주기적으로 트랜잭션을 생성하며, 트랜잭션의 생성은 BL_ProtocolLayerPoS 인스턴스의 `_txgentimer` 타이머 오브젝트에 의하여 주기적으로 수행된다. 또한 PoS 로직에 따라 일정한 간격 (`Slot`)마다 블록을 생성할 리더를 결정하며, 리더 선택 로직은 BL_ProtocolLayerPoS 인스턴스의 `_slottimer` 타이머 오브젝트에 의하여 주기적으로 수행된다.
## Transaction Generation
## Slot Leader Selection
