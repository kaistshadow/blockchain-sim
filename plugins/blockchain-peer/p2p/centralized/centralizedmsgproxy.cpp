#include "centralizedmsgproxy.h"
#include "peerlistmanager.h"
#include "simplesocketinterface.h"
#include "centralizednetworkmessage.h"
#include "../../blockchain/txpool.h"
#include "../../consensus/simpleconsensus.h"
#include "../../util/globalclock.h"
#include "../../util/hexstring.h"

#include <boost/variant.hpp>

CentralizedMessageProxy* CentralizedMessageProxy::instance = 0;

CentralizedMessageProxy* CentralizedMessageProxy::GetInstance() {
    if (instance == 0) {
        instance = new CentralizedMessageProxy();
    }
    return instance;
}

// CentralizedMessageProxy::Broadcast(CentralizedP2PMessage ) : Broadcast a P2PMessage
// This function assumes simple centralized network topology.
// Thus, this broadcast function simply transfers P2P message to a centralized p2p node. (i.e., node 1)
// When centralized node receive a P2PMessage, centralized node will transfer the message 
// all over the network.
//  Also, for simplicity, I make it as a function, instead of a queue-based communication.
// TODO : Change the communication methodology. In other words, we need to unify the communication mechanism
// between modules. For example, queue-based communication.

// void CentralizedMessageProxy::Broadcast(CentralizedP2PMessage msg) {
//     NetworkMessage netmsg(NetworkMessage_P2PMSG, msg);
//     SimpleSocketInterface::GetInstance()->SendNetworkMsg(netmsg, "bleep1"); // hardcoded centralized node. TODO: make it flexible.
// }



// This function is called by centralized node.
void CentralizedMessageProxy::SendOverP2PNetwork(ProxyGeneratedMessage& msg) {
    CentralizedNetworkMessage cmsg(CentralizedNetworkMessage_PROXYGENERATEDMSG, msg);    

    PeerList& outPeerList = SimplePeerListManager::GetInstance()->GetOutPeerList();
    
    for (Peer* outPeer : outPeerList) {
        if (outPeer->conn_status == CONNECTED) {
            std::cout << "SendOverP2PNetwork: send to " << outPeer->hostname << "\n"; 
            SimpleSocketInterface::GetInstance()->SendNetworkMsg(cmsg, outPeer->hostname);
        }
        else {
            std::cout << "Not connected outPeer" << "\n";
        }
    }
}

// void CentralizedMessageProxy::ProcessQueue() : handles request for broadcasting or unicast
// Only the centralized node utilizes this proxy class and its queue.
// Decapsulate the message, and send them over p2p networks.
void CentralizedMessageProxy::ProcessQueue() {
    while (!broadcastMsgQueue.empty()) {
        BroadcastRequestMessage& msg = broadcastMsgQueue.front();
        ProxyGeneratedMessage pmsg;
        pmsg.type = msg.type;
        pmsg.data = msg.data;

        if (pmsg.type == ProxyGeneratedMessage_POWCONSENSUSMESSAGE) {
            POWConsensusMessage powmsg = boost::get<POWConsensusMessage>(msg.data);
            if (powmsg.type == POWConsensusMessage_NEWBLOCK) {
                POWBlock blk = boost::get<POWBlock>(powmsg.value);

                std::string rawhash = blk.GetBlockHash().str().substr(0,6);
                std::string hashval = utility::HexStr(rawhash);
                std::cout << utility::GetGlobalClock() << ":CentralizedMessageProxy: broadcast newblock " << ": block hash=" << hashval << "\n"; 
            }
        }
        
        SendOverP2PNetwork(pmsg);

        broadcastMsgQueue.pop();
    }

    while (!unicastMsgQueue.empty()) {
        UnicastRequestMessage& msg = unicastMsgQueue.front();
        ProxyGeneratedMessage pmsg;

        // bool send = true;
        // switch(msg.type) {
        // case UnicastRequestMessage_TRANSACTION:
        //     {
        //         Transaction tx = boost::get<Transaction>(msg.data);
        //         pmsg.type = ProxyGeneratedMessage_TRANSACTION;
        //         pmsg.data = tx;
        //         break;
        //     }
        // case UnicastRequestMessage_BLOCK:
        //     {
        //         Block blk = boost::get<Block>(msg.data);
        //         pmsg.type = ProxyGeneratedMessage_BLOCK;
        //         pmsg.data = blk;
        //         break;
        //     }
        // case UnicastRequestMessage_SIMPLECONSENSUSMESSAGE:
        //     {
        //         SimpleConsensusMessage smsg = boost::get<SimpleConsensusMessage>(msg.data);
        //         pmsg.type = ProxyGeneratedMessage_SIMPLECONSENSUSMESSAGE;
        //         pmsg.data = smsg;
        //         break;
        //     }
        // case UnicastRequestMessage_POWCONSENSUSMESSAGE:
        //     {
        //         POWConsensusMessage powmsg = boost::get<POWConsensusMessage>(msg.data);
        //         pmsg.type = ProxyGeneratedMessage_POWCONSENSUSMESSAGE;

        //         POWConsensusMessage newpowmsg;
        //         switch(powmsg.type) {
        //         case POWConsensusMessage_NEWBLOCK:
        //             {
        //                 newpowmsg.type = POWConsensusMessage_NEWBLOCK;
        //                 newpowmsg.value = boost::get<POWBlock>(powmsg.value);
        //                 newpowmsg.msg_sender = powmsg.msg_sender;
        //                 break;
        //             }
        //         case POWConsensusMessage_REQBLOCKS:
        //             {
        //                 newpowmsg.type = POWConsensusMessage_REQBLOCKS;
        //                 newpowmsg.value = POWConsensusMessage_REQBLOCKS;  // dummy value. but wrong behavior if not assigned
        //                 newpowmsg.msg_sender = powmsg.msg_sender;
        //                 break;
        //             }
        //         case POWConsensusMessage_RESPBLOCKS:
        //             {
        //                 POWBlocks blks = boost::get<POWBlocks>(powmsg.value);
        //                 for (auto received_blk : blks) {
        //                     std::cout << "CentralizedMessageProxy: RESPBLOCKS msg's block: "<< received_blk.GetNonce() << "\n";
        //                 }
        //                 newpowmsg.type = POWConsensusMessage_RESPBLOCKS;
        //                 newpowmsg.value = blks;
        //                 newpowmsg.msg_sender = powmsg.msg_sender;
        //                 break;
        //             }
        //         }
        //         pmsg.data = newpowmsg;
        //         break;
        //     }
        // }
        pmsg.type = msg.type;
        pmsg.data = msg.data;
        
        CentralizedNetworkMessage cmsg(CentralizedNetworkMessage_PROXYGENERATEDMSG, pmsg);    
        if (pmsg.type == ProxyGeneratedMessage_POWCONSENSUSMESSAGE) {
            POWConsensusMessage powmsg = boost::get<POWConsensusMessage>(msg.data);
            if (powmsg.type == POWConsensusMessage_RESPBLOCKS) {
                POWBlocks blks = boost::get<POWBlocks>(powmsg.value);
                std::string rawhash = blks.back().GetBlockHash().str().substr(0,6);
                std::string hashval = utility::HexStr(rawhash);
                std::cout << utility::GetGlobalClock() << ":CentralizedMessageProxy: send blockresp to " << msg.dest << ": last block hash=" << hashval << "\n"; 
            }
        }
        std::cout << "CentralizedMessageProxy: send to " << msg.dest << "\n"; 
        SimpleSocketInterface::GetInstance()->SendNetworkMsg(cmsg, msg.dest);
        
        unicastMsgQueue.pop();
    }
}
