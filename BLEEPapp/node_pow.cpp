#include "mainmodules/MainEventManager.h"
#include "networkmodules/BasicNetworkModule.h"
#include "datamodules/Peer.h"
#include "datamodules/Message.h"

#include "utility/ArgsManager.h"

#include "datamanagermodules/TxPool.h"
#include "datamodules/POWBlock.h"
#include "consensusmodules/POWModule.h"
#include "datamanagermodules/ListLedgerManager.h"

#include <chrono>
#include <ctime>
#include <time.h>
#include <math.h>
#include <sys/time.h>

using namespace libBLEEP;

// Need to implement event loop library

// implement event loop's wait

// implement event loop's appendNewNeighborPeer, GenerateTx

// TODO(0331) : step-by-step visualization, implement asyncGenerateTx
// TODO(0331) : SendMessage, recvMessage
// TODO(0401) : Change PeerManager to PeerConnManager. Add NewPeerConnected event.
// TODO(0402) : Support disconnect API
// TODO(0402) : Refactoring for socket closeEvent handling (use recvMsg instead of recv?)

// v1.0
// TODO : Seperate API src,dest & msg src,dest (for broadcasting portability)
// TODO : statmachine regtest
// TODO : documentation for (Transaction, TxPool), (Block,LedgerManager), (PeerId, Message)
//        (MainEventManager_v1), utility ArgsManager

// TODO : connected neighbor peer already exists handling. -> remove redundent datasocket
// TODO : NewPeerConnected event

// 

std::shared_ptr<POWBlock> MakeCandidateBlock(TxPool& txPool, ListLedgerManager<POWBlock>& ledger) {
    int txNumPerBlock = std::stoi(gArgs.GetArg("-blocktxnum"));

    std::list<boost::shared_ptr<Transaction> > txs = txPool.GetTxs(txNumPerBlock); 
    std::shared_ptr<POWBlock> candidateBlk = std::make_shared<POWBlock>("", txs);

    unsigned long blockidx = ledger.GetNextBlockIdx();
    candidateBlk->SetBlockIdx(blockidx);

    POWBlock *lastBlock = ledger.GetLastBlock();
    if (lastBlock) {
        candidateBlk->SetPrevBlockHash(lastBlock->GetBlockHash());
    }
    else {
        candidateBlk->SetPrevBlockHash(UINT256_t(0xffffff)); // no valid prev block hash since it's genesis block
    }
    return candidateBlk;
}

void AppendBlockToLedger(std::shared_ptr<POWBlock> validBlk, TxPool& txPool, ListLedgerManager<POWBlock>& ledger) {
    txPool.RemoveTxs(validBlk->GetTransactions());
    std::cout << GetGlobalClock() << ":valid block appended" << "\n";
    std::cout << *(std::shared_ptr<Block>)validBlk << "\n";
    ledger.AppendBlock(validBlk);
}


int main(int argc, char *argv[]) {
    // for testing DisconnectPeer API
    int receivedMessageCount = 0;

    gArgs.ParseParameters(argc, argv);

    if (gArgs.IsArgSet("-?") || gArgs.IsArgSet("-h") ||  gArgs.IsArgSet("-help") || gArgs.IsArgSet("-version")) {
        std::string strUsage = gArgs.HelpMessage();
        std::cout << strUsage << "\n";
        return 0;
    }

    /* allocate mainEventManager */
    MainEventManager mainEventManager;
    // MainEventManager mainEventManager(gArgs.GetArg("-id", "noid"));
    BasicNetworkModule basicNetworkModule(gArgs.GetArg("-id", "noid"), &mainEventManager);

    // POWModule
    POWModule powModule(&mainEventManager);

    // transaction pool
    TxPool txPool;

    // ListLedgerManager
    ListLedgerManager<POWBlock> ledger;

    /* connect to peer */
    // mainEventManager.AsyncConnectPeer(PeerId("143.248.38.37"));
    for (auto neighborPeerId : gArgs.GetArgs("-connect"))
        basicNetworkModule.AsyncConnectPeer(PeerId(neighborPeerId));

    while(true) {
        mainEventManager.Wait(); // main event loop (wait for next event)
        
        switch (mainEventManager.GetEventType()) {
        case AsyncEventEnum::none:
            std::cout << "invalid event is triggered. " << "\n";
            exit(-1);
        case AsyncEventEnum::CompleteAsyncConnectPeer:
            {
                std::cout << "event for connection complete for peer. " << "\n";
                PeerId peerId = mainEventManager.GetAsyncEventDataManager().GetConnectedPeerId();
                std::cout << "connected peerId : " << peerId.GetId() << "\n";
                // mainEventManager.SendMessage(message);
                break;
            }
        case AsyncEventEnum::ErrorAsyncConnectPeer:
            {
                std::cout << "AsyncConnectPeer got error(" << mainEventManager.GetAsyncEventDataManager().GetError() << ")" << "\n";
                // try again with timer
                PeerId peerId = mainEventManager.GetAsyncEventDataManager().GetRefusedPeerId();                
                basicNetworkModule.AsyncConnectPeer(peerId, 10);
                break;
            }
        case AsyncEventEnum::CompleteAsyncGenerateRandomTransaction:
            {
                std::cout << "random transaction generated" << "\n";
                boost::shared_ptr<Transaction> generatedTx = mainEventManager.GetAsyncEventDataManager().GetGeneratedTx();
                std::cout << *generatedTx << "\n";
                break;
            }
        case AsyncEventEnum::RecvMessage:
            {
                std::cout << "RecvMessage" << "\n";
                std::shared_ptr<Message> msg = mainEventManager.GetAsyncEventDataManager().GetReceivedMsg();
                MessageType messageType = msg->GetType();
                if (messageType == "newTx") {
                    boost::shared_ptr<Transaction> receivedTx = GetDeserializedTransaction(msg->GetPayload());
                    std::cout << *receivedTx << "\n";
                    txPool.AddTx(receivedTx);
                    int txNumPerBlock = std::stoi(gArgs.GetArg("-blocktxnum"));
                    if (txPool.GetPendingTxNum() >= txNumPerBlock && !powModule.IsMining()) {
                        std::shared_ptr<POWBlock> candidateBlk = MakeCandidateBlock(txPool, ledger);
                        double mining_avg = std::stof(gArgs.GetArg("-miningtime"));
                        double mining_dev = std::stof(gArgs.GetArg("-miningtimedev"));
                        powModule.AsyncEmulateBlockMining(candidateBlk, mining_avg, mining_dev);
                    }
                } 
                else if (messageType == "newBlock") {
                    std::shared_ptr<Block> receivedBlk = GetDeserializedBlock(msg->GetPayload());  
                    std::cout << "received newBlock" << "\n";
                    std::cout << *receivedBlk << "\n";

                    std::shared_ptr<POWBlock> receivedPOWBlk = std::dynamic_pointer_cast<POWBlock> (receivedBlk); // we know it's POWBlock
                    M_Assert(receivedPOWBlk != nullptr, "it should be POWBlock");

                    unsigned long nextblkidx = ledger.GetNextBlockIdx();
                    POWBlock* lastblk = ledger.GetLastBlock();
                    if (lastblk == nullptr ||
                        (lastblk->GetBlockHash() == receivedPOWBlk->GetPrevBlockHash() && nextblkidx == receivedPOWBlk->GetBlockIdx()) ) {
                        /* received valid new blk message from neighbor */
                        /* Thus, stop the mining, and append a received blk to my ledger */

                        // stop mining
                        if (powModule.IsMining()) {
                            powModule.StopMining();
                        }

                        // append a block to ledger
                        AppendBlockToLedger(receivedPOWBlk, txPool, ledger);

                        // restart mining for new block
                        int txNumPerBlock = std::stoi(gArgs.GetArg("-blocktxnum"));
                        if (txPool.GetPendingTxNum() >= txNumPerBlock) {
                            std::shared_ptr<POWBlock> candidateBlk = MakeCandidateBlock(txPool, ledger);
                            double mining_avg = std::stof(gArgs.GetArg("-miningtime"));
                            double mining_dev = std::stof(gArgs.GetArg("-miningtimedev"));
                            powModule.AsyncEmulateBlockMining(candidateBlk, mining_avg, mining_dev);
                        }
                    } 
                    else if (nextblkidx <= receivedPOWBlk->GetBlockIdx()) {
                        /* received valid new blk message from neighbor,
                           and it is from longer blockchain than my blockchain.
                           Thus, request a new message for blocks of longer blockchain */
                        std::cout << GetGlobalClock() << ":Block (sented by " << msg->GetSource().GetId() << ") is received and longer than mine" << "\n";
                        std::cout << "Need to implement ReqBlocks, RespBlocks" << "\n";
                    }
                    else {
                        std::cout << GetGlobalClock() << ":Block is received but not appended" << "\n";
                    }
                }

                // // for testing DisconnectPeer API
                // receivedMessageCount++;
                // if (receivedMessageCount >= 5) {
                //     for (auto neighborPeerId : gArgs.GetArgs("-connect"))
                //         basicNetworkModule.DisconnectPeer(PeerId(neighborPeerId));
                // }
                break;
            }
        case AsyncEventEnum::NewPeerConnected:
            {
                std::shared_ptr<PeerId> newConnectedNeighbor = mainEventManager.GetAsyncEventDataManager().GetNewlyConnectedPeer();
                std::cout << "NewPeerConnected requested from " << newConnectedNeighbor->GetId() << "\n";

                break;
            }
        case AsyncEventEnum::PeerDisconnected:
            {
                std::shared_ptr<PeerId> disconnectedNeighbor = mainEventManager.GetAsyncEventDataManager().GetDisconnectedPeerId();
                std::cout << "Disconnection requested from " << disconnectedNeighbor->GetId() << "\n";

                break;
            }
        case AsyncEventEnum::EmuBlockMiningComplete:
            {
                std::cout << "block mining complte" << "\n";
                std::shared_ptr<POWBlock> minedBlk = mainEventManager.GetAsyncEventDataManager().GetMinedBlock();
                AppendBlockToLedger(minedBlk, txPool, ledger);

                // propagate to network
                PeerId myPeerId(gArgs.GetArg("-id"));
                std::string payload = GetSerializedString(minedBlk);
                for (auto neighborPeerId : gArgs.GetArgs("-connect")) {
                    PeerId destPeerId(neighborPeerId);
                    std::shared_ptr<Message> msg = std::make_shared<Message>(myPeerId, destPeerId, 
                                                                             "newBlock", payload);
                    basicNetworkModule.UnicastMessage(destPeerId, msg);
                }

                break;
            }
        }
    }
}
