#include "mainmodules/MainEventManager.h"
#include "mainmodules/RandomGossipNetworkModule.h"
#include "mainmodules/POWModule.h"
#include "mainmodules/TxGeneratorModule.h"

#include "datamodules/Peer.h"
#include "datamodules/Message.h"
#include "datamodules/POWBlock.h"

#include "datamanagermodules/TxPool.h"
#include "datamanagermodules/TreeLedgerManager.h"

#include "utility/ArgsManager.h"
#include "utility/GlobalClock.h"
#include "utility/Logger.h"

#include "shadow_interface.h"

using namespace libBLEEP;

static void printHash(std::string hash) {
    gLog << "blockhash:" << UINT256_t((const unsigned char*)hash.c_str(), 32) << "\n";
    return;
}

void  genPeerList(std::vector<PeerId> &lst, std::string myId, int maxPeerNum){
    for(int i = 0; i < maxPeerNum; i++){
        char name[10];
        sprintf(name, "bleep%d", i);
        if(myId == name) break;
        lst.push_back(PeerId(name));
    }
}

#include <chrono>
#include <ctime>
#include <time.h>
#include <math.h>
#include <sys/time.h>

int mined_block_num = 0;

// TODO : Seperate API src,dest & msg src,dest (for broadcasting portability)
// TODO [DONE] : statmachine regtest
// TODO [DONE] : documentation for (Transaction, TxPool), (Block,LedgerManager), (PeerId, Message)
//        (MainEventManager_v1), utility ArgsManager

// TODO [DONE] : connected neighbor peer already exists handling. -> remove redundent datasocket
// TODO [DONE]: NewPeerConnected event



std::shared_ptr<POWBlock> MakeCandidateBlock(TxPool& txPool, TreeLedgerManager<POWBlock>& ledger) {
    int txNumPerBlock = std::stoi(gArgs.GetArg("-blocktxnum"));

    std::list<boost::shared_ptr<Transaction> > txs = txPool.GetTxs(txNumPerBlock); 
    std::shared_ptr<POWBlock> candidateBlk = std::make_shared<POWBlock>("", txs);

    unsigned long blockidx = ledger.GetNextBlockIdx();
    candidateBlk->SetBlockIdx(blockidx);
    candidateBlk->SetPrevBlockHash(ledger.GetLastHash());

    return candidateBlk;
}

void AppendBlockToLedger(std::shared_ptr<POWBlock> validBlk, TxPool& txPool, TreeLedgerManager<POWBlock>& ledger) {
    txPool.RemoveTxs(validBlk->GetTransactions());
    ledger.AppendBlock(validBlk);
}

// bool UpdateLedgerAsLongestChain(std::vector<POWBlock>& received_blks, TxPool& txPool, ListLedgerManager<POWBlock>& ledger) {
//     unsigned long ledger_lastblkidx = ledger.GetLastBlock()->GetBlockIdx();
//     unsigned long received_lastblkidx = received_blks.back().GetBlockIdx();
    
//     if (received_lastblkidx <= ledger_lastblkidx)
//         return false;
    
//     std::list<POWBlock>& ledger_blks = ledger.GetLedger();
//     std::list<POWBlock>::iterator ledger_it = ledger_blks.begin();
//     std::vector<POWBlock>::iterator received_blks_it = received_blks.begin();

//     // This while loop is for the situation where the received blks is partial blockchain
//     while (ledger_it != ledger_blks.end() &&
//            ledger_it->GetBlockIdx() != received_blks_it->GetBlockIdx()) {
//         ledger_it++;
//     }

//     // Now blocks pointed by ledger_it and received_blks_it have same index
    
//     while (ledger_it != ledger_blks.end() && received_blks_it != received_blks.end() &&
//            ledger_it->GetBlockHash() == received_blks_it->GetBlockHash()) {
//         ledger_it++;
//         received_blks_it++;
//     }
//     // Now blocks pointed by ledger_it and received_blks_it have same index but different hash

//     if (received_blks_it != received_blks.end()) {

//         // update transaction pool
//         std::for_each(ledger_it, ledger_blks.end(), 
//                       [&txPool](POWBlock &blk) { txPool.AddTxs(blk.GetTransactions()); });
//         std::for_each(received_blks_it, received_blks.end(),
//                       [&txPool](POWBlock &blk) { txPool.RemoveTxs(blk.GetTransactions()); });

//         // // replace ledger
//         ledger.ReplaceLedger(ledger_it, ledger_blks.end(), received_blks_it, received_blks.end());

//         return true;
//     }
//     else
//         return false;
// }

int main(int argc, char *argv[]) {
    std::vector<PeerId> peerList;

    // test for computation latency emulation
    // init_shadow_clock_update();
    // int a = 0;
    // for (int i = 0; i < 100000000; i++)
    //     a++;
    // gLog << a << "\n";
    // double loopmilli = next_shadow_clock_update();
    // gLog << "time for executing loop:" << loopmilli << "\n";


    // for testing DisconnectPeer API

    gArgs.ParseParameters(argc, argv);

    if (gArgs.IsArgSet("-?") || gArgs.IsArgSet("-h") ||  gArgs.IsArgSet("-help") || gArgs.IsArgSet("-version")) {
        std::string strUsage = gArgs.HelpMessage();
        gLog << strUsage << "\n";
        return 0;
    }

    int connectPeerNum = std::stoi(gArgs.GetArg("-outpeernum"));;
    int fanOut = std::stoi(gArgs.GetArg("-fanout"));

    /* allocate mainEventManager */
    MainEventManager mainEventManager;
    // MainEventManager mainEventManager(gArgs.GetArg("-id", "noid"));

    std::string myId = gArgs.GetArg("-id", "noid");
    RandomGossipNetworkModule randomNetworkModule(myId, &mainEventManager, fanOut);

    // int maxPeerNum = 500;
    // genPeerList(peerList, myId, maxPeerNum);
    for (auto neighborPeerId : gArgs.GetArgs("-connect")) {
        if (myId != neighborPeerId)
            peerList.push_back(PeerId(neighborPeerId));
    }

    randomNetworkModule.AsyncConnectPeers(peerList, connectPeerNum);

    // BasicNetworkModule basicNetworkModule(gArgs.GetArg("-id", "noid"), &mainEventManager);

    // Transaction generator module
    TxGeneratorModule txGeneratorModule(&mainEventManager);

    // POWModule
    POWModule powModule(gArgs.GetArg("-id", "noid"), &mainEventManager);

    // transaction pool
    TxPool txPool;

    // TreeLedgerManager
    TreeLedgerManager<POWBlock> ledger;

    std::shared_ptr<POWBlock> candidateBlk = std::make_shared<POWBlock>();
    candidateBlk->SetBlockIdx(0);
    candidateBlk->SetBlockHash(UINT256_t(0xffffff));

    // insert genesis Block 
    ledger.AppendBlock(candidateBlk);


    /* connect to peer */
    // mainEventManager.AsyncConnectPeer(PeerId("143.248.38.37"));
    if (gArgs.IsArgSet("-txgeninterval")) {
        txGeneratorModule.AsyncGenerateRandomTransaction(std::stof(gArgs.GetArg("-txgeninterval")));
    } else {
        int txNumPerBlock = std::stoi(gArgs.GetArg("-blocktxnum"));
        
        // add txs to transaction pool
        struct timespec tspec;
        clock_gettime(CLOCK_MONOTONIC, &tspec);
        unsigned int randtime = (tspec.tv_sec + tspec.tv_nsec);
        gLog << "randtime:" << randtime << "\n";
        srand(randtime);
        for (int i = 0; i < txNumPerBlock*105; i++) {
            int sender_id = rand() % 100;
            int receiver_id = rand() % 100;
            float amount = (float) (rand() % 10000);
            boost::shared_ptr<Transaction> generatedTx(new SimpleTransaction(sender_id, receiver_id, amount));

            gLog << *generatedTx << "\n";
            txPool.AddTx(generatedTx);
        }
        gLog << "TxPool added, cur size:" << txPool.GetPendingTxNum() << "\n";
        
        // start mining
        std::shared_ptr<POWBlock> candidateBlk = MakeCandidateBlock(txPool, ledger);
        double mining_avg = std::stof(gArgs.GetArg("-miningtime"));
        double mining_dev = std::stof(gArgs.GetArg("-miningtimedev"));
        powModule.AsyncEmulateBlockMiningGaussian(candidateBlk, mining_avg, mining_dev);
    }

    std::map<std::string, bool> reconnectTry;
    while(true) {
        mainEventManager.Wait(); // main event loop (wait for next event)
        
        PrintTimespec("mainEventManager.Wait returned");
        while (mainEventManager.ExistAsyncEvent()) {
            AsyncEvent event = mainEventManager.PopAsyncEvent();
        
            switch (event.GetType()) {
            case AsyncEventEnum::none:
                gLog << "invalid event is triggered. " << "\n";
                exit(-1);
            case AsyncEventEnum::CompleteAsyncConnectPeer:
                {
                    gLog << "event for connection complete for peer. " << "\n";
                    PeerId peerId = event.GetData().GetConnectedPeerId();
                    gLog << "connected peerId : " << peerId.GetId() << "\n";
                    // mainEventManager.SendMessage(message);
                    break;
                }
            case AsyncEventEnum::ErrorAsyncConnectPeer:
                {
                    gLog << "AsyncConnectPeer got error(" << event.GetData().GetError() << ")" << "\n";
                    // try again with timer
                    PeerId peerId = event.GetData().GetRefusedPeerId();                

                    if (reconnectTry[peerId.GetId()] == false) {
                        randomNetworkModule.AsyncConnectPeer(peerId, 1); // retry once
                        reconnectTry[peerId.GetId()] = true;
                    }
                    break;
                }
            case AsyncEventEnum::CompleteAsyncGenerateRandomTransaction:
                {
                    // gLog << "random transaction generated" << "\n";
                    boost::shared_ptr<Transaction> generatedTx = event.GetData().GetGeneratedTx();
                    // gLog << *generatedTx << "\n";

                    txPool.AddTx(generatedTx);
                    int txNumPerBlock = std::stoi(gArgs.GetArg("-blocktxnum"));
                    if (txPool.GetPendingTxNum() >= txNumPerBlock && !powModule.IsMining()) {
                        std::shared_ptr<POWBlock> candidateBlk = MakeCandidateBlock(txPool, ledger);
                        double mining_avg = std::stof(gArgs.GetArg("-miningtime"));
                        double mining_dev = std::stof(gArgs.GetArg("-miningtimedev"));
                        powModule.AsyncEmulateBlockMiningGaussian(candidateBlk, mining_avg, mining_dev);
                    }

                    // Call another request, i.e., periodically generate transaction
                    txGeneratorModule.AsyncGenerateRandomTransaction(std::stof(gArgs.GetArg("-txgeninterval")));

                    // double milli = next_shadow_clock_update();
                    // gLog << "time for handling CompleteAsyncGenerateRandomTransaction:" << milli << "\n";
                    break;
                }
            case AsyncEventEnum::RecvMessage:
                {
                    // gLog << "RecvMessage" << "\n";
                    PrintTimespec("mainEventLoop AsyncEventEnum::recvMessage");
                    std::shared_ptr<Message> msg = event.GetData().GetReceivedMsg();
                    std::shared_ptr<PeerId> msgFrom = event.GetData().GetReceivedFromPeerId();
                    MessageType messageType = msg->GetType();
                    if (messageType == "newTx") {
                        boost::shared_ptr<Transaction> receivedTx = GetDeserializedTransaction(msg->GetPayload());
                        gLog << *receivedTx << "\n";
                        txPool.AddTx(receivedTx);
                        int txNumPerBlock = std::stoi(gArgs.GetArg("-blocktxnum"));
                        if (txPool.GetPendingTxNum() >= txNumPerBlock && !powModule.IsMining()) {
                            std::shared_ptr<POWBlock> candidateBlk = MakeCandidateBlock(txPool, ledger);
                            double mining_avg = std::stof(gArgs.GetArg("-miningtime"));
                            double mining_dev = std::stof(gArgs.GetArg("-miningtimedev"));
                            powModule.AsyncEmulateBlockMiningGaussian(candidateBlk, mining_avg, mining_dev);
                        }
                    } 
                    else if (messageType == "Inventory") {
                        gLog << "Inventory msg received" << "\n";
                        gLog << "received from:" << msg->GetSource().GetId() << "\n";

                        Inventory inv = GetDeserializedInventory(msg->GetPayload());
                        const std::list<std::string>& hashlist = inv.GetHashlist();
                        gLog << "inventory hashlist size:" << hashlist.size() << "\n";

                        for (auto h : hashlist) {
                            printHash(h);
                        }
                        
                        if (ledger.HasProcessingInv())
                            break;

                        //unsigned long nextblkidx = ledger.GetNextBlockIdx();
                        UINT256_t lasthash = ledger.GetLastHash();

                        // First, synchronize global hash tree

                        // check whether the getblocks is needed
                        if (!ledger.ContainBlockHash(hashlist.front())) {
                            
                            gLog << "before getblocklocator" << "\n";
                            struct timespec start,end;
                            shadow_clock_gettime(CLOCK_MONOTONIC, &start);
                            std::vector<std::string> blockLocator = ledger.GetBlockLocator();
                            shadow_clock_gettime(CLOCK_MONOTONIC, &end);
                            double nanoseconds = end.tv_nsec >= start.tv_nsec
                                ? (end.tv_nsec - start.tv_nsec) + (end.tv_sec - start.tv_sec) * 1e9
                                : (start.tv_nsec - end.tv_nsec) + (end.tv_sec - start.tv_sec - 1) * 1e9;
                            gLog << "millisec:" << (int)(nanoseconds/1000) << "\n";


                            gLog << "after getblocklocator" << "\n";
                            for (std::string h : blockLocator) {
                                gLog << "hash in blocklocator:" << h << "\n";
                            }
                            
                            POWConsensusMessage powmsg("getblocks", blockLocator);
                            
                            // propagate to network
                            PeerId myPeerId(gArgs.GetArg("-id"));
                            PeerId destPeerId = *msgFrom;
                            std::string payload = GetSerializedString(powmsg);
                            std::shared_ptr<Message> msg = std::make_shared<Message>(myPeerId, destPeerId, 
                                                                                     "POWConsensusProtocol", payload);
                            randomNetworkModule.UnicastMessage(destPeerId, msg);

                        } else {
                            // valid hash list. Thus, synchronize the hash tree
                            // for (std::string h : hashlist) {
                            //     ledger.AppendBlockHash(h);
                            // }

                            // Register processing inventory to ledgermanager
                            ledger.SetProcessingInv(inv);
                            ledger.StartProcessingInv();


                            // Second, if hash tree lacks of actual block, request the block
                            bool containAll = true;
                            for (std::string h : hashlist) {
                                if (!ledger.ContainBlock(h)) {
                                    gLog << "send POW getdata message" << "\n";
                                    POWConsensusMessage powmsg("getdata", h);
                                    // propagate to network
                                    PeerId myPeerId(gArgs.GetArg("-id"));
                                    PeerId destPeerId = *msgFrom;
                                    std::string payload = GetSerializedString(powmsg);
                                    std::shared_ptr<Message> msg = std::make_shared<Message>(myPeerId, destPeerId, 
                                                                                             "POWConsensusProtocol", payload);
                                    randomNetworkModule.UnicastMessage(destPeerId, msg);
                                    containAll = false;
                                    break; // request only a single block
                                }
                                else 
                                    gLog << "ledger contains" << h << "\n";
                            }

                            if (containAll) {
                                gLog << "stop processing inv" << "\n";
                                ledger.StopProcessingInv();
                            }

                            M_Assert( lasthash == ledger.GetLastHash(), "Lasthash should not be changed");
                        }
                    }
                    // else if (messageType == "newBlock") {
                    //     std::shared_ptr<Block> receivedBlk = GetDeserializedBlock(msg->GetPayload());  
                    //     // gLog << "received newBlock" << "\n";

                    //     std::shared_ptr<POWBlock> receivedPOWBlk = std::dynamic_pointer_cast<POWBlock> (receivedBlk); // we know it's POWBlock
                    //     M_Assert(receivedPOWBlk != nullptr, "it should be POWBlock");

                    //     // add timestamp
                    //     struct timespec tspec;
                    //     clock_gettime(CLOCK_MONOTONIC, &tspec);
                    //     blocktimelogs[msg->GetMessageId()]["BlockReceived"] = tspec;

                    //     unsigned long nextblkidx = ledger.GetNextBlockIdx();
                    //     UINT256_t lasthash = ledger.GetLastHash();

                    //     if (nextblkidx == 0 ||
                    //         lasthash == receivedPOWBlk->GetPrevBlockHash()) {

                    //         // stop mining
                    //         if (powModule.IsMining()) {
                    //             powModule.StopMining();
                    //         }
                    //         // append a block to ledger
                    //         AppendBlockToLedger(receivedPOWBlk, txPool, ledger);

                    //         // restart mining for new block
                    //         int txNumPerBlock = std::stoi(gArgs.GetArg("-blocktxnum"));
                    //         if (txPool.GetPendingTxNum() >= txNumPerBlock) {
                    //             std::shared_ptr<POWBlock> candidateBlk = MakeCandidateBlock(txPool, ledger);
                    //             double mining_avg = std::stof(gArgs.GetArg("-miningtime"));
                    //             double mining_dev = std::stof(gArgs.GetArg("-miningtimedev"));
                    //             powModule.AsyncEmulateBlockMiningGaussian(candidateBlk, mining_avg, mining_dev);
                    //         }
                    //     }
                    //     else {
                    //         /* received orphan or stale block */

                    //         // add timestamp
                    //         struct timespec tspec;
                    //         clock_gettime(CLOCK_MONOTONIC, &tspec);
                    //         blocktimelogs[msg->GetMessageId()]["ForkBlockReceived"] = tspec;
                    //         AppendBlockToLedger(receivedPOWBlk, txPool, ledger);
                    //     }
                    // }
                    else if (messageType == "POWConsensusProtocol") {
                        gLog << "received POWConsensusProtocol message" << "\n";
                        POWConsensusMessage receivedPOWmsg = GetDeserializedPOWConsensusMessage(msg->GetPayload());


                        if (receivedPOWmsg.GetType() == "getdata") {
                            gLog << "received POWConsensusProtocol getdata message" << "\n";

                            std::string blockhash = receivedPOWmsg.GetHash();
                            // std::vector<POWBlock> blks;
                        
                            // if (ledger.ContainBlock(blockhash))
                            //     blks.push_back(*ledger.GetBlock(blockhash));
                            M_Assert(ledger.ContainBlock(blockhash), "hash must be synchronized already.");
                                     
                            POWConsensusMessage blockmsg("block", ledger.GetBlock(blockhash));

                            // propagate to network
                            PeerId myPeerId(gArgs.GetArg("-id"));
                            PeerId destPeerId = msg->GetSource();
                            std::string payload = GetSerializedString(blockmsg);
                            std::shared_ptr<Message> msg = std::make_shared<Message>(myPeerId, destPeerId, 
                                                                                     "POWConsensusProtocol", payload);
                            randomNetworkModule.UnicastMessage(destPeerId, msg);
                        }
                        else if (receivedPOWmsg.GetType() == "block") {
                            gLog << "received POWConsensusProtocol block message" << "\n";

                            std::shared_ptr<POWBlock> blkptr = receivedPOWmsg.GetPOWBlockPtr();                            

                            UINT256_t lasthash = ledger.GetLastHash();
                            // append a block to ledger
                            gLog << "blockhash:" << blkptr->GetBlockHash().str() << "\n";
                            if (!ledger.ContainBlock(blkptr->GetBlockHash().str()))  {
                                AppendBlockToLedger(blkptr, txPool, ledger);
                            }

                            // if lasthash is changed, restart mining
                            if (lasthash != ledger.GetLastHash()) {
                                // stop mining
                                if (powModule.IsMining()) {
                                    powModule.StopMining();
                                }
                                // restart mining for new block
                                int txNumPerBlock = std::stoi(gArgs.GetArg("-blocktxnum"));
                                if (txPool.GetPendingTxNum() >= txNumPerBlock) {
                                    std::shared_ptr<POWBlock> candidateBlk = MakeCandidateBlock(txPool, ledger);
                                    double mining_avg = std::stof(gArgs.GetArg("-miningtime"));
                                    double mining_dev = std::stof(gArgs.GetArg("-miningtimedev"));
                                    powModule.AsyncEmulateBlockMiningGaussian(candidateBlk, mining_avg, mining_dev);
                                }
                            }

                            if (ledger.HasProcessingInv()) {
                                Inventory inv = ledger.GetProcessingInv();

                                const std::list<std::string>& hashlist = inv.GetHashlist();

                                // if hash tree lacks of actual block, request the block
                                // else, finish the inv processing.
                                bool invAllProcessed = true;
                                for (std::string h : hashlist) {
                                    if (!ledger.ContainBlock(h)) {
                                        gLog << "POWConsensusMessage getdata sent for" << h << "\n";
                                        POWConsensusMessage powmsg("getdata", h);
                                        // propagate to network
                                        PeerId myPeerId(gArgs.GetArg("-id"));
                                        PeerId destPeerId = msg->GetSource();
                                        std::string payload = GetSerializedString(powmsg);
                                        std::shared_ptr<Message> msg = std::make_shared<Message>(myPeerId, destPeerId, 
                                                                                                 "POWConsensusProtocol", payload);
                                        randomNetworkModule.UnicastMessage(destPeerId, msg);
                                        invAllProcessed = false;
                                        break; // request only a single block
                                    }
                                }

                                if (invAllProcessed) {
                                    gLog << "stop processing inv" << "\n";
                                    ledger.StopProcessingInv();

                                    // create blk hash message
                                    PeerId src(gArgs.GetArg("-id"));
                                    PeerId dest = PeerId("Multicast");
                                    std::string payload = GetSerializedString(inv);
                                    std::shared_ptr<Message> nMsg = std::make_shared<Message>(src, dest, "Inventory", payload);
                                    // propagate to network
                                    randomNetworkModule.ForwardMessage(nMsg, std::make_shared<PeerId>(src));
                                    randomNetworkModule.InsertMessageSet(nMsg->GetMessageId());
                                }
                            }
                        } 
                        else if (receivedPOWmsg.GetType() == "getblocks") {
                            gLog << "received POWConsensusProtocol getblocks message" << "\n";
                            std::vector<std::string> blockLocator = receivedPOWmsg.GetHashlist();

                            std::string commonBlkHash = "";
                            commonBlkHash = ledger.FindCommonBlock(blockLocator);
                            // for (std::string h : blockLocator) {
                            //     if (ledger.ContainBlock(h)) {
                            //         commonBlkHash = h;
                            //         break;
                            //     }
                            // }
                            M_Assert(commonBlkHash != "", "At least, genesis is common block");
                               
                            Inventory inv;

                            gLog << "received from:" << msg->GetSource().GetId() << "\n";
                            gLog << "commonBlkHash:" << commonBlkHash << "\n";

                            std::vector<std::string> chain = ledger.GetChainFromBlock(commonBlkHash);
                            
                            for (std::vector<std::string>::reverse_iterator it = chain.rbegin();
                                 it != chain.rend(); it++) {
                                inv.AppendHash(*it);
                            }
                            // TODO: received peer should implement Initial Block Download Mechanism (IBD)

                            // std::string childBlkHash = ledger.GetChildBlockHash(commonBlkHash);
                            // for (int i = 0; i < 500; i++) {
                            //     if (childBlkHash == "")
                            //         break;
                            //     inv.AppendHash(childBlkHash);
                            //     childBlkHash = ledger.GetChildBlockHash(childBlkHash);
                            // }

                            gLog << "inventory size:" << inv.GetHashlist().size() << "\n";

                            // create blk hash message
                            PeerId src(gArgs.GetArg("-id"));
                            PeerId dest = msg->GetSource();
                            std::string payload = GetSerializedString(inv);
                            std::shared_ptr<Message> nMsg = std::make_shared<Message>(src, dest, "Inventory", payload);

                            randomNetworkModule.UnicastMessage(dest, nMsg);

                        }
                    }
                    // // for testing DisconnectPeer API
                    // receivedMessageCount++;
                    // if (receivedMessageCount >= 5) {
                    //     for (auto neighborPeerId : gArgs.GetArgs("-connect"))
                    //         basicNetworkModule.DisconnectPeer(PeerId(neighborPeerId));
                    // }
                    // double milli = next_shadow_clock_update("handling RecvMessage");
                    // gLog << "time for handling RecvMessage:" << milli << "\n";

                    break;
                }
            case AsyncEventEnum::NewPeerConnected:
                {
                    std::shared_ptr<PeerId> newConnectedNeighbor = event.GetData().GetNewlyConnectedPeer();
                    gLog << "NewPeerConnected requested from " << newConnectedNeighbor->GetId() << "\n";

                    break;
                }
            case AsyncEventEnum::PeerDisconnected:
                {
                    std::shared_ptr<PeerId> disconnectedNeighbor = event.GetData().GetDisconnectedPeerId();
                    gLog << "Disconnection requested from " << disconnectedNeighbor->GetId() << "\n";

                    break;
                }
            case AsyncEventEnum::EmuBlockMiningComplete:
                {
                    // char buf[256];
                    // sprintf(buf, "EmuBlockMiningComplete,%d,%lu",
                    //         mined_block_num, ledger.GetNextBlockIdx() - 1);
                    // shadow_push_eventlog(buf);

                    // init_shadow_clock_update();
                    gLog << "block mining complte" << "\n";
                    std::shared_ptr<POWBlock> minedBlk = event.GetData().GetMinedBlock();

                    Inventory inv;
                    gLog << "blockhash:" << minedBlk->GetBlockHash() << "\n";
                    gLog << "blockhash(str):" << minedBlk->GetBlockHash().str() << "\n";
                    gLog << "blockhash:" << UINT256_t((const unsigned char*)minedBlk->GetBlockHash().str().c_str(), 32) << "\n";

                    inv.AppendHash(minedBlk->GetPrevBlockHash().str());
                    inv.AppendHash(minedBlk->GetBlockHash().str());

                    const std::list<std::string>& hashlist = inv.GetHashlist();
                    gLog << "sending inventory hashlist size:" << hashlist.size() << "\n";
                    for (auto h : hashlist) {
                        printHash(h);
                    }

                    // create blk hash message
                    PeerId src(gArgs.GetArg("-id"));
                    PeerId dest = PeerId("Multicast");
                    std::string payload = GetSerializedString(inv);
                    std::shared_ptr<Message> nMsg = std::make_shared<Message>(src, dest, "Inventory", payload);

                    std::stringstream str, str2;
                    std::ostringstream ss, ss2;
                    str << minedBlk->GetBlockHash();
                    ss << str.rdbuf();
                    std::string blockHash = ss.str();

                    str2 << minedBlk->GetPrevBlockHash();
                    ss2 << str2.rdbuf();
                    std::string prevBlockHash = ss2.str();


                    char buf2[256];
                    sprintf(buf2, "EmuBlockMiningComplete,%s,%s,%f,%lu,%s",
                            blockHash.substr(2, 10).c_str(),
                            prevBlockHash.substr(2,10).c_str(),
                            minedBlk->GetTimestamp(),
                            ledger.GetNextBlockIdx(), 
                            nMsg->GetMessageId().c_str());
                    shadow_push_eventlog(buf2);


                    gLog << "Inventory message created" << "\n";
                    // add timestamp
                    struct timespec tspec;
                    clock_gettime(CLOCK_MONOTONIC, &tspec);
                    blocktimelogs[nMsg->GetMessageId()]["BlockMined"] = tspec;

                    // append block to ledger
                    AppendBlockToLedger(minedBlk, txPool, ledger);
                    mined_block_num++;

                    gLog << "block appended" << "\n";

                    // restart mining timer
                    int txNumPerBlock = std::stoi(gArgs.GetArg("-blocktxnum"));
                    if (txPool.GetPendingTxNum() >= txNumPerBlock && !powModule.IsMining()) {
                        std::shared_ptr<POWBlock> candidateBlk = MakeCandidateBlock(txPool, ledger);
                        double mining_avg = std::stof(gArgs.GetArg("-miningtime"));
                        double mining_dev = std::stof(gArgs.GetArg("-miningtimedev"));
                        powModule.AsyncEmulateBlockMiningGaussian(candidateBlk, mining_avg, mining_dev);
                    }

                    // propagate to network
                    randomNetworkModule.MulticastMessage(nMsg);
                    randomNetworkModule.InsertMessageSet(nMsg->GetMessageId());


                    // gLog << "mined block num = " << mined_block_num << "\n";

                    // if (ledger.GetNextBlockIdx() == 101) {
                    //     PrintBlockTimeLogs();

                    //     gLog << "total_mined_block_num=" << mined_block_num << "\n";
                    //     char buf[256];
                    //     sprintf(buf, "ResultStat,%s,%d,%lu",
                    //             "TotalMinedBlockNum",
                    //             mined_block_num - 1, ledger.GetNextBlockIdx() - 1);
                    //     shadow_push_eventlog(buf);

                        
                    //     exit(0);
                    // }

                    break;
                }
            }
        }
    }
}
