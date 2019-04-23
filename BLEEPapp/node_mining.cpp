#include "mainmodules/MainEventManager.h"
#include "mainmodules/BasicNetworkModule.h"
#include "mainmodules/POWModule.h"

#include "datamodules/Peer.h"
#include "datamodules/Message.h"
#include "datamodules/POWBlock.h"

#include "datamanagermodules/TxPool.h"
#include "datamanagermodules/ListLedgerManager.h"

#include "utility/ArgsManager.h"

using namespace libBLEEP;


// TODO : Seperate API src,dest & msg src,dest (for broadcasting portability)
// TODO [DONE] : statmachine regtest
// TODO [DONE] : documentation for (Transaction, TxPool), (Block,LedgerManager), (PeerId, Message)
//        (MainEventManager_v1), utility ArgsManager

// TODO [DONE] : connected neighbor peer already exists handling. -> remove redundent datasocket
// TODO [DONE]: NewPeerConnected event



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

    // ListLedgerManager
    ListLedgerManager<POWBlock> ledger;

    std::shared_ptr<POWBlock> candidateBlk = std::make_shared<POWBlock>();
    unsigned char default_th[32] = {0x00,0x00,0x0f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
    UINT256_t difficulty(default_th, 32);
    powModule.AsyncBlockMining(candidateBlk, difficulty);    

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
                    break;
                }
            case AsyncEventEnum::ErrorAsyncConnectPeer:
                {
                    std::cout << "AsyncConnectPeer got error(" << event.GetData().GetError() << ")" << "\n";
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
                    }

                    break;
                }
            case AsyncEventEnum::EmuBlockMiningComplete:
                {
                    std::cout << "block mining complte" << "\n";
                    break;
                }
            case AsyncEventEnum::BlockMiningComplete:
                {
                    std::cout << "block mining complte" << "\n";
                    std::shared_ptr<POWBlock> minedBlk = event.GetData().GetMinedBlock();
                    ledger.AppendBlock(minedBlk);
                    break;
                }
            }
        }
    }
}
