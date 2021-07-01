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
외부로부터 블록 Inventory 메세지 `POWBLOCK-INV`를 받는 경우 `_RecvPOWBlockInvHandler` 함수가 수행된다. 해당 함수의 동작은 다음과 같다.
- 받은 Inventoy 메세지에 존재하는 Hash (TODO)
## Block Request Handling
외부로부터 블록 요청 메시지 `POWBLOCK-GETDATA`를 받는 경우 `_RecvPOWBlockGetDataHandler` 함수가 수행된다. 해당 함수의 동작은 다음과 같다.
- 블록 요청 메시지에 존재하는 블록 해시값에 일치하는 블록 데이터를 자신의 블록 트리에서 가져와 블록 데이터 메시지(`POWBLOCK-BLK`) 구성
- 구성되 메시지를 요청을 보낸 피어에게 전송
## Block Data Response Handling
외부로부터 블록 데이터 메시지 `POWBLOCK-BLK`를 받는 경우 `_RecvPOWBlockBlkHandler` 함수가 수행된다. 해당 함수의 동작은 다음과 같다.
- 자신의 블록 트리에 해당 블록 데이터가 포함되어있지 않은 경우 해당 블록 데이터를 블록 트리에 추가하고 블록 내 트랜잭션들을 트랜잭션 풀에서 제외시킴
- 블록 트리에서 가장 긴 블록 체인이 변화하는 경우, 블록 마이닝을 초기화하고 새로 시작
- 해당 블록체인 노드에서 처리중인 블록 Inventory 메세지가 존재하는 경우, 각 Inventory 메시지에 대응되는 블록 요청 메세지를 블록 데이터를 보낸 피어에게 전송
- 모든 Inventory 메세지가 처리된 경우, Inventory 처리를 중지하고 자신의 이웃 피어들에게 블록 Inventory 메세지를 전송하여 블록 전파 매커니즘을 수행

# Timer Function Detail
BLEEP PoW node는 주기적으로 트랜잭션 및 블록을 생성하며, 트랜잭션의 생성은 `BL_ProtocolLayerPoW` 인스턴스의 `_txgentimer` 타이머 오브젝트, 블록 마이닝은 POWMiner 인스턴스의 `_timer` 타이머 오브젝트에 의하여 주기적으로 수행된다.
## Transaction Generation
`txGenInterval` 파라미터에 의하여 트랜잭션 생성 주기가 정해지며, 생성 주기마다 `BL_ProtocolLayerPoW` 인스턴스의 `_txgentimerCallback` 함수가 수행된다. 해당 함수의 동작은 다음과 같다.
- 랜덤한 수신자 id, 송신자 id, 전송 값으로 임의의 트랜잭션을 생성
- 생성한 트랜잭션을 자신의 트랜잭션 풀에 저장
- 트랜잭션을 자신의 이웃 피어에게 브로드캐스트
## Block Generation
블록 생성은 POWMiner 인스턴스 `_powMiner`의 `AsyncEmulateBlockMining` 함수에 의하여 수행된다. 해당 함수의 동작은 다음과 같다.
- 평균 블록 생성 시간 파라미터를 기반으로 지수분포의 랜덤값 생성
- 해당 랜덤값만큼 시간이 지난 후 발생하는 타이머 이벤트 등록
- 해당 타이머 이벤트에 사용할 마이닝 예정 블록을 저장해놓음
`AsyncEmulateBlockMining`에서 등록된 마이닝 예정 블록은 타이머 이벤트 수행시 타임스탬프, 블록 해시값 및 랜덤한 nonce 값이 설정되며, 타이머 이벤트의 결과로 `MainEventManager` 인스턴스에 `EmuBlockMiningComplete` 이벤트를 발생시킨다.
발생한 `EmuBlockMiningComplete`의 결과로 자신의 블록체인에 새로 마이닝된 블록이 추가되며, 이웃 피어들에게 해당 블록에 대한 Inventory 메시지가 전파된다.

