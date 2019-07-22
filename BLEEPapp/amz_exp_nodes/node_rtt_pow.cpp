#include "mainmodules/MainEventManager.h"
#include "mainmodules/EstimateRTTModule.h"
#include "mainmodules/POWModule.h"
#include "mainmodules/TxGeneratorModule.h"

#include "datamodules/Peer.h"
#include "datamodules/Message.h"
#include "datamodules/POWBlock.h"

#include "datamanagermodules/TxPool.h"
#include "datamanagermodules/ListLedgerManager.h"

#include "utility/ArgsManager.h"
#include "utility/GlobalClock.h"

#include "shadow_interface.h"

using namespace libBLEEP;

#include <chrono>
#include <ctime>
#include <time.h>
#include <math.h>
#include <sys/time.h>

int mined_block_num = 0;

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

        return true;
    }
    else
        return false;
}


int main(int argc, char *argv[]) {
    gArgs.ParseParameters(argc, argv);

    if (gArgs.IsArgSet("-?") || gArgs.IsArgSet("-h") ||  gArgs.IsArgSet("-help") || gArgs.IsArgSet("-version")) {
        std::string strUsage = gArgs.HelpMessage();
        std::cout << strUsage << "\n";
        return 0;
    }

    /* allocate mainEventManager */
    MainEventManager mainEventManager;
    // MainEventManager mainEventManager(gArgs.GetArg("-id", "noid"));

    std::string myId = gArgs.GetArg("-id", "noid");
    double rttSendTime = std::stof(gArgs.GetArg("-rttstart", "0.0"));
    int sendNum = std::stoi(gArgs.GetArg("-rttnum", "0"));
    EstimateRTTModule estimateRTTModule(myId, rttSendTime, sendNum, &mainEventManager, 0);

    // int maxPeerNum = 500;
    // genPeerList(peerList, myId, maxPeerNum);
    for (auto neighborPeerId : gArgs.GetArgs("-connect")) {
        estimateRTTModule.AsyncConnectPeer(PeerId(neighborPeerId));
    }

    // Transaction generator module
    TxGeneratorModule txGeneratorModule(&mainEventManager);

    // POWModule
    POWModule powModule(gArgs.GetArg("-id", "noid"), &mainEventManager);

    // BasicNetworkModule basicNetworkModule(gArgs.GetArg("-id", "noid"), &mainEventManager);

    // transaction pool
    TxPool txPool;

    // ListLedgerManager
    ListLedgerManager<POWBlock> ledger;

    if (gArgs.IsArgSet("-txgeninterval")) {
        txGeneratorModule.AsyncGenerateRandomTransaction(std::stof(gArgs.GetArg("-txgeninterval")));
    } else {
        int txNumPerBlock = std::stoi(gArgs.GetArg("-blocktxnum"));
        
        // add txs to transaction pool
        struct timespec tspec;
        clock_gettime(CLOCK_MONOTONIC, &tspec);
        unsigned int randtime = (tspec.tv_sec + tspec.tv_nsec);
        std::cout << "randtime:" << randtime << "\n";
        srand(randtime);
        for (int i = 0; i < txNumPerBlock*105; i++) {
            int sender_id = rand() % 100;
            int receiver_id = rand() % 100;
            float amount = (float) (rand() % 10000);
            boost::shared_ptr<Transaction> generatedTx(new SimpleTransaction(sender_id, receiver_id, amount));

            std::cout << *generatedTx << "\n";
            txPool.AddTx(generatedTx);
        }
        std::cout << "TxPool added, cur size:" << txPool.GetPendingTxNum() << "\n";
        
        // start mining
        std::shared_ptr<POWBlock> candidateBlk = MakeCandidateBlock(txPool, ledger);
        double mining_avg = std::stof(gArgs.GetArg("-miningtime"));
        double mining_dev = std::stof(gArgs.GetArg("-miningtimedev"));
        powModule.AsyncEmulateBlockMining(candidateBlk, mining_avg, mining_dev);
    }


    std::map<std::string, bool> reconnectTry;
    while(true) {
        mainEventManager.Wait(); // main event loop (wait for next event)
        
        PrintTimespec("mainEventManager.Wait returned");
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

                    if (reconnectTry[peerId.GetId()] == false) {
                        estimateRTTModule.AsyncConnectPeer(peerId, 1); // retry once
                        reconnectTry[peerId.GetId()] = true;
                    }
                    break;
                }
            case AsyncEventEnum::CompleteAsyncGenerateRandomTransaction:
                {
                    // std::cout << "random transaction generated" << "\n";
                    boost::shared_ptr<Transaction> generatedTx = event.GetData().GetGeneratedTx();
                    // std::cout << *generatedTx << "\n";

                    txPool.AddTx(generatedTx);
                    int txNumPerBlock = std::stoi(gArgs.GetArg("-blocktxnum"));
                    if (txPool.GetPendingTxNum() >= txNumPerBlock && !powModule.IsMining()) {
                        std::shared_ptr<POWBlock> candidateBlk = MakeCandidateBlock(txPool, ledger);
                        double mining_avg = std::stof(gArgs.GetArg("-miningtime"));
                        double mining_dev = std::stof(gArgs.GetArg("-miningtimedev"));
                        powModule.AsyncEmulateBlockMining(candidateBlk, mining_avg, mining_dev);
                    }

                    // Call another request, i.e., periodically generate transaction
                    txGeneratorModule.AsyncGenerateRandomTransaction(std::stof(gArgs.GetArg("-txgeninterval")));

                    // double milli = next_shadow_clock_update();
                    // std::cout << "time for handling CompleteAsyncGenerateRandomTransaction:" << milli << "\n";
                    break;
                }
            case AsyncEventEnum::RecvMessage:
                {
                    std::cout << "RecvMessage" << "\n";
                    PrintTimespec("mainEventLoop AsyncEventEnum::recvMessage");
                    std::shared_ptr<Message> msg = event.GetData().GetReceivedMsg();
                    MessageType messageType = msg->GetType();

                    if (messageType == "RTTReq") { 
                        std::cout << "received Req message" << "\n";
                    } else if (messageType == "newBlock") {
                        std::shared_ptr<Block> receivedBlk = GetDeserializedBlock(msg->GetPayload());  
                        // std::cout << "received newBlock" << "\n";

                        std::shared_ptr<POWBlock> receivedPOWBlk = std::dynamic_pointer_cast<POWBlock> (receivedBlk); // we know it's POWBlock
                        M_Assert(receivedPOWBlk != nullptr, "it should be POWBlock");

                        // add timestamp
                        struct timespec tspec;
                        clock_gettime(CLOCK_MONOTONIC, &tspec);
                        blocktimelogs[msg->GetMessageId()]["BlockReceived"] = tspec;

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
                            // std::cout << GetGlobalClock() << ":Block (sented by " << msg->GetSource().GetId() << ") is received and longer than mine" << "\n";
                            // std::cout << "Need to implement ReqBlocks, RespBlocks" << "\n";
                            POWConsensusMessage powmsg("REQBLOCKS");
                        
                            // add timestamp
                            struct timespec tspec;
                            clock_gettime(CLOCK_MONOTONIC, &tspec);
                            blocktimelogs[msg->GetMessageId()]["ForkBlockReceived"] = tspec;

                            // propagate to network
                            PeerId myPeerId(gArgs.GetArg("-id"));
                            PeerId destPeerId = msg->GetSource();
                            std::string payload = GetSerializedString(powmsg);
                            std::shared_ptr<Message> msg = std::make_shared<Message>(myPeerId, destPeerId, 
                                                                                     "POWConsensusProtocol", payload);
                            estimateRTTModule.UnicastMessage(destPeerId, msg);
                        }
                        else {
                            // add timestamp
                            struct timespec tspec;
                            clock_gettime(CLOCK_MONOTONIC, &tspec);
                            blocktimelogs[msg->GetMessageId()]["ForkBlockReceived"] = tspec;
                            // std::cout << GetGlobalClock() << ":Block is received but not appended" << "\n";
                        }
                    } 
                    else if (messageType == "POWConsensusProtocol") {
                        POWConsensusMessage receivedPOWmsg = GetDeserializedPOWConsensusMessage(msg->GetPayload());  
                        // std::cout << "received POWConsensusProtocol message" << "\n";
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
                            estimateRTTModule.UnicastMessage(destPeerId, msg);
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
                    // init_shadow_clock_update();

                    // std::cout << "block mining complte" << "\n";
                    std::shared_ptr<POWBlock> minedBlk = event.GetData().GetMinedBlock();

                    // // add timestamp
                    // struct timespec tspec;
                    // clock_gettime(CLOCK_MONOTONIC, &tspec);
                    // blocktimelogs[nMsg->GetMessageId()]["BlockMined"] = tspec;

                    // append block to ledger
                    AppendBlockToLedger(minedBlk, txPool, ledger);
                    mined_block_num++;

                    // restart mining timer
                    int txNumPerBlock = std::stoi(gArgs.GetArg("-blocktxnum"));
                    if (txPool.GetPendingTxNum() >= txNumPerBlock && !powModule.IsMining()) {
                        std::shared_ptr<POWBlock> candidateBlk = MakeCandidateBlock(txPool, ledger);
                        double mining_avg = std::stof(gArgs.GetArg("-miningtime"));
                        double mining_dev = std::stof(gArgs.GetArg("-miningtimedev"));
                        powModule.AsyncEmulateBlockMining(candidateBlk, mining_avg, mining_dev);
                    }



                    PeerId src(gArgs.GetArg("-id"));
                    std::string payload = GetSerializedString(minedBlk);
                    for (auto neighborPeerId: gArgs.GetArgs("-connect")) {
                        if (neighborPeerId == gArgs.GetArg("-id"))
                            continue;
                        PeerId destPeerId(neighborPeerId);

                        // create blk message
                        std::shared_ptr<Message> nMsg = std::make_shared<Message>(src, destPeerId, "newBlock", payload);

                        estimateRTTModule.UnicastMessage(destPeerId, nMsg);
                    }
                    // // propagate to network
                    // randomNetworkModule.MulticastMessage(nMsg);
                    // randomNetworkModule.InsertMessageSet(nMsg->GetMessageId());



                    // std::cout << "mined block num = " << mined_block_num << "\n";
                    if (ledger.GetNextBlockIdx() == 101) {
                        PrintBlockTimeLogs();

                        std::cout << "total_mined_block_num=" << mined_block_num << "\n";
                        char buf[256];
                        sprintf(buf, "ResultStat,%s,%d,%lu",
                                "TotalMinedBlockNum",
                                mined_block_num - 1, ledger.GetNextBlockIdx() - 1);
                        shadow_push_eventlog(buf);

                        
                        exit(0);
                    }

                    // next_shadow_clock_update("==== done processing EmuBlockMiningComplete Event");
                    shadow_usleep(1000);
                    PrintTimespec("EmuBlockMiningComplete processing ended");
                    // double milli = next_shadow_clock_update("===== handling EmuBlockMiningComplete");
                    break;
                }
            }
        }
    }
}
