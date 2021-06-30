# BLEEP Blockchain Library Proof of Work Abstraction
`BLEEPlib`은 시뮬레이션을 위한 합의 알고리즘 중 하나로 PoW Abstraction을 제공한다. PoW Abstraction 관련 처리는 `ProtocolLayerPoW.cpp`, `ProtocolLayerPoW.h` 파일에서 이뤄지며, 수행되는 내용은 다음과 같다.
- PoW 관련 메세지 핸들링: 수신된 Inventory 메세지, Block 요청 메세지, Transaction 요청 메세지, 블록 전송 메세지 등을 처리
- 타이머 함수 처리: 주기적인 트랜잭션 생성 및 블록 마이닝 로직 수행

# Message Handling Function Detail
외부로부터 수신한 Protocol layer 관련 메세지를 핸들링 하는 코드는 다음과 같다 (`ProtocolLayerPoW.cpp`).
```
void BL_ProtocolLayerPoW::RecvMsgHandler(std::shared_ptr<Message> msg) {
    MessageType msgType = msg->GetType();
    if (msgType == "TXGOSSIP-INV") {
        _txGossipProtocol.RecvInventoryHandler(msg);
    } else if (msgType == "TXGOSSIP-GETDATA") {
        _txGossipProtocol.RecvGetdataHandler(msg);
    } else if (msgType == "TXGOSSIP-TXS") {
        _txGossipProtocol.RecvTxsHandler(msg);
        if (_txPool->GetPendingTxNum() >= txNumPerBlock) {
            if (!_powMiner.IsMining()) {
                std::shared_ptr<POWBlock> candidateBlk = _makeCandidateBlock();
                _powMiner.AsyncEmulateBlockMining(candidateBlk, 1/miningtime/miningnodecnt);
            }
        }
    } else if (msgType == "POWBLOCK-INV") {
        _RecvPOWBlockInvHandler(msg);
    } else if (msgType == "POWBLOCK-GETBLOCKS") {
        _RecvPOWBlockGetBlocksHandler(msg);
    } else if (msgType == "POWBLOCK-GETDATA") {
        _RecvPOWBlockGetDataHandler(msg);
    } else if (msgType == "POWBLOCK-BLK") {
        _RecvPOWBlockBlkHandler(msg);
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
BLEEP PoW node는 주기적으로 트랜잭션 및 블록을 생성하며, 트랜잭션의 생성은 BL_ProtocolLayerPoW 인스턴스의 `_txgentimer` 타이머 오브젝트, 블록 마이닝은 POWMiner 인스턴스의 `_timer` 타이머 오브젝트에 의하여 주기적으로 수행된다.
## Transaction Generation
## Block Generation
