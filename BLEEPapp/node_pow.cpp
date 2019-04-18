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
    ledger.AppendBlock(validBlk);
}

bool UpdateLedgerAsLongestChain(std::vector<POWBlock>& received_blks, TxPool& txPool, ListLedgerManager<POWBlock>& ledger) {
    unsigned long ledger_lastblkidx = ledger.GetLastBlock()->GetBlockIdx();
    unsigned long received_lastblkidx = received_blks.back().GetBlockIdx();
    
    if (received_lastblkidx <= ledger_lastblkidx)
        return false;
    
    std::list<POWBlock>& ledger_blks = ledger.GetLedger();
    std::list<POWBlock>::iterator ledger_it = ledger_blks.begin();
    std::vector<POWBlock>::iterator received_blks_it = received_blks.begin();

    // This while loop is for the situation where the received blks is partial blockchain
    while (ledger_it != ledger_blks.end() &&
           ledger_it->GetBlockIdx() != received_blks_it->GetBlockIdx()) {
        ledger_it++;
    }

    // Now blocks pointed by ledger_it and received_blks_it have same index
    
    while (ledger_it != ledger_blks.end() && received_blks_it != received_blks.end() &&
           ledger_it->GetBlockHash() == received_blks_it->GetBlockHash()) {
        ledger_it++;
        received_blks_it++;
    }
    // Now blocks pointed by ledger_it and received_blks_it have same index but different hash

    if (received_blks_it != received_blks.end()) {

        // update transaction pool
        std::for_each(ledger_it, ledger_blks.end(), 
                      [&txPool](POWBlock &blk) { txPool.AddTxs(blk.GetTransactions()); });
        std::for_each(received_blks_it, received_blks.end(),
                      [&txPool](POWBlock &blk) { txPool.RemoveTxs(blk.GetTransactions()); });

        // // replace ledger
        ledger.ReplaceLedger(ledger_it, ledger_blks.end(), received_blks_it, received_blks.end());

        // while (ledger_it != ledger_blks.end()) {
        //     // txPool.AddTxs(ledger_it->GetTransactions());
        //     ledger_it = ledger_blks.erase(ledger_it);
        // }

        // while (received_blks_it != received_blks.end()) {
        //     // txPool.RemoveTxs(received_blks_it->GetTransactions());
        //     ledger_blks.push_back(*received_blks_it);
        //     std::cout << "Longest chain block is appended" << "\n";
        //     std::cout << *received_blks_it << "\n";
        //     received_blks_it++;
        // }

        return true;
    }
    else
        return false;
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
    ListLedgerManager<POWBlock> ledger(gArgs.GetArg("-id", "noid"));

    /* connect to peer */
    // mainEventManager.AsyncConnectPeer(PeerId("143.248.38.37"));
    for (auto neighborPeerId : gArgs.GetArgs("-connect"))
        basicNetworkModule.AsyncConnectPeer(PeerId(neighborPeerId));

    while(true) {
        mainEventManager.Wait(); // main event loop (wait for next event)
        
        while (mainEventManager.ExistAsyncEvent()) {
            AsyncEvent event = mainEventManager.PopAsyncEvent();
        
            switch (event.GetType()) {
            case AsyncEventEnum::none:
                std::cout << "invalid event is triggered. " << "\n";
                exit(-1);
            case AsyncEventEnum::CompleteAsyncConnectPeer:
                {
                    std::cout << "event for connection complete for peer. " << "\n";
                    PeerId peerId = event.GetData().GetConnectedPeerId();
                    std::cout << "connected peerId : " << peerId.GetId() << "\n";
                    // mainEventManager.SendMessage(message);
                    break;
                }
            case AsyncEventEnum::ErrorAsyncConnectPeer:
                {
                    std::cout << "AsyncConnectPeer got error(" << event.GetData().GetError() << ")" << "\n";
                    // try again with timer
                    PeerId peerId = event.GetData().GetRefusedPeerId();                
                    basicNetworkModule.AsyncConnectPeer(peerId, 10);
                    break;
                }
            case AsyncEventEnum::CompleteAsyncGenerateRandomTransaction:
                {
                    std::cout << "random transaction generated" << "\n";
                    boost::shared_ptr<Transaction> generatedTx = event.GetData().GetGeneratedTx();
                    std::cout << *generatedTx << "\n";
                    break;
                }
            case AsyncEventEnum::RecvMessage:
                {
                    std::cout << "RecvMessage" << "\n";
                    std::shared_ptr<Message> msg = event.GetData().GetReceivedMsg();
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
                            POWConsensusMessage powmsg("REQBLOCKS");
                        
                            // propagate to network
                            PeerId myPeerId(gArgs.GetArg("-id"));
                            PeerId destPeerId = msg->GetSource();
                            std::string payload = GetSerializedString(powmsg);
                            std::shared_ptr<Message> msg = std::make_shared<Message>(myPeerId, destPeerId, 
                                                                                     "POWConsensusProtocol", payload);
                            basicNetworkModule.UnicastMessage(destPeerId, msg);
                        }
                        else {
                            std::cout << GetGlobalClock() << ":Block is received but not appended" << "\n";
                        }
                    } 
                    else if (messageType == "POWConsensusProtocol") {
                        POWConsensusMessage receivedPOWmsg = GetDeserializedPOWConsensusMessage(msg->GetPayload());  
                        std::cout << "received POWConsensusProtocol message" << "\n";
                        if (receivedPOWmsg.GetType() == "REQBLOCKS") {
                            std::vector<POWBlock> blks;
                            std::list<POWBlock>& ledgerBlks = ledger.GetLedger();
                            for (auto blk : ledgerBlks) {
                                blks.push_back(blk);
                            }
                            POWConsensusMessage newPOWmsg("RESPBLOCKS", blks);

                            // propagate to network
                            PeerId myPeerId(gArgs.GetArg("-id"));
                            PeerId destPeerId = msg->GetSource();
                            std::string payload = GetSerializedString(newPOWmsg);
                            std::shared_ptr<Message> msg = std::make_shared<Message>(myPeerId, destPeerId, 
                                                                                     "POWConsensusProtocol", payload);
                            basicNetworkModule.UnicastMessage(destPeerId, msg);
                        } 
                        else if (receivedPOWmsg.GetType() == "RESPBLOCKS") {
                            std::vector<POWBlock>& blks = receivedPOWmsg.GetPOWBlocks();
                            if (UpdateLedgerAsLongestChain(blks, txPool, ledger)) {
                                if (powModule.IsMining()) {
                                    powModule.StopMining();
                                    // restart mining for new block
                                    int txNumPerBlock = std::stoi(gArgs.GetArg("-blocktxnum"));
                                    if (txPool.GetPendingTxNum() >= txNumPerBlock) {
                                        std::shared_ptr<POWBlock> candidateBlk = MakeCandidateBlock(txPool, ledger);
                                        double mining_avg = std::stof(gArgs.GetArg("-miningtime"));
                                        double mining_dev = std::stof(gArgs.GetArg("-miningtimedev"));
                                        powModule.AsyncEmulateBlockMining(candidateBlk, mining_avg, mining_dev);
                                    }
                                }
                            }
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
                    std::shared_ptr<PeerId> newConnectedNeighbor = event.GetData().GetNewlyConnectedPeer();
                    std::cout << "NewPeerConnected requested from " << newConnectedNeighbor->GetId() << "\n";

                    break;
                }
            case AsyncEventEnum::PeerDisconnected:
                {
                    std::shared_ptr<PeerId> disconnectedNeighbor = event.GetData().GetDisconnectedPeerId();
                    std::cout << "Disconnection requested from " << disconnectedNeighbor->GetId() << "\n";

                    break;
                }
            case AsyncEventEnum::EmuBlockMiningComplete:
                {
                    std::cout << "block mining complte" << "\n";
                    std::shared_ptr<POWBlock> minedBlk = event.GetData().GetMinedBlock();
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
}