#include "HandleNetwork_GossipModel.h"
#include "../datamodules/Peer.h"
#include "../event/GlobalEvent.h"
#include "../Configuration.h"
#include "../utility/NodeInfo.h"
#include "../utility/GlobalClock.h"

#include <assert.h>
#include <iostream>
#include <iomanip>
#include <ev.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <strings.h>
#include <stdio.h>
#include <sys/socket.h>
#include <vector>
#include <tr1/functional>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/serialization/export.hpp>

BOOST_CLASS_EXPORT(GossipModuleMessage);

/* --- singleton pattern --- */
HandleNetwork_GossipModel* HandleNetwork_GossipModel::instance = 0;
HandleNetwork_GossipModel* HandleNetwork_GossipModel::GetInstance() {
    if (instance == 0) {
        instance = new HandleNetwork_GossipModel();
    }
    return instance;
}

/* ------ Functions related to class HandleNetwork_GossipModel ----- */
int HandleNetwork_GossipModel::InitializeListenSocket() {
    int socketfd = socket(AF_INET, (SOCK_STREAM | SOCK_NONBLOCK), 0);
    if (socketfd == -1) {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in my_addr;
    my_addr.sin_family = AF_INET;
    my_addr.sin_port   = htons(MYPORT);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(my_addr.sin_zero), 8);

    if (bind(socketfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {
        perror("bind");
        close(socketfd);
        exit(1);
    }
    if (listen(socketfd, BACKLOG) == -1) {
        perror("listen");
        close(socketfd);
        exit(1);
    }
    return socketfd;
}

void HandleNetwork_GossipModel::onRecvSocketConnectionEvent(std::shared_ptr<EventInfo> info) {
    std::shared_ptr<RecvSocketConnectionEventInfo> derivedInfo = std::static_pointer_cast<RecvSocketConnectionEventInfo>(info);
    int fd = derivedInfo->GetListenFD();

    struct sockaddr_in client_addr;
    socklen_t sin_size = sizeof(struct sockaddr_in);

    int receiver_fd = accept(fd, (struct sockaddr *)&client_addr, &sin_size);
    if (receiver_fd == -1 ) {
        std::cout << "[onRecvSocketConnectionEvent] errno=" << errno << strerror(errno) << "\n";
        return;
    }

    int flags = fcntl(receiver_fd, F_GETFL, 0);
    fcntl(receiver_fd, F_SETFL, flags | O_NONBLOCK);

    Peer* peer = new Peer(receiver_fd, std::string(inet_ntoa(client_addr.sin_addr)));
    PeerMap[receiver_fd] = peer;
    socketEventPublisher.RegisterSocketAsDataSocket(receiver_fd);
    TimeoutManager.RegisterTimeoutWatcher(receiver_fd);
    
    #if LOGGING
        std::cout << "[onRecvSocketConnectionEvent] got connection from "<< peer->GetIP() << "\n";
    #endif
}

void HandleNetwork_GossipModel::onRecvSocketDataEvent(std::shared_ptr<EventInfo> info) {
    std::shared_ptr<RecvSocketDataEventInfo> derivedInfo = std::static_pointer_cast<RecvSocketDataEventInfo>(info);
    
	int  fd = derivedInfo->GetSocketFD();
    auto it = PeerMap.find(fd);
    if (it == PeerMap.end()) {
        std::cout << "[onRecvSocketDataEvent] No valid peer exists\n";
        exit(-1);
    }

    Peer* peer = it->second;
    SocketRecvStatus& status = recvStatusMap[fd];
    char string_read[2000];
    bool is_disconnect = false;

    switch (status.recv_status) {
        case STATUS_IDLE:
            {
                int length = 0;
                int numbytes = recv(fd, &length, sizeof(int), 0);
                if (numbytes == -1 && errno != EAGAIN){
                    std::cout << "[onRecvSocketDataEvent] IDLE errno "<< errno <<"\n";
                    exit(-1);
                }
                else if (numbytes == 0) {
                    is_disconnect = true;
                }
                else if (numbytes > 0) {
                    status.message_len  = length;
                    status.recv_status  = STATUS_HEADER;
                    status.received_len = 0;
                    status.recv_str     = "";

                    #if LOGGING
                        std::cout << "RECV\n";
                    #endif
                }
            }
            break;

        case STATUS_HEADER:
        case STATUS_MSG:
            {
                int numbytes = 0;
                int total_recv_size = status.received_len;
                while(1) {
                    int recv_size = std::min(2000, status.message_len - total_recv_size);
                    numbytes = recv(fd, string_read, recv_size, 0);
                    if (numbytes > 0) {
                        status.recv_str.append(string_read, numbytes);
                        total_recv_size += numbytes;
                        if (total_recv_size == status.message_len) break;
                    }
                    else if (numbytes == 0) {
                        is_disconnect = true;
                        break;
                    }
                    else {
                        std::cout << "[onRecvSocketDataEvent] HEADER/MSG recv message fail\n";
                        break;
                    }
                    memset(string_read, 0, 2000);
                }

                if (status.message_len != total_recv_size) {
                    status.received_len = total_recv_size;
                    std::cout << "[onRecvSocketDataEvent] HEADER/MSG received only some part\n";
                    break;
                }

                #if LOGGING
                        std::cout << "RECV\n";
                #endif
             
                if (status.recv_status == STATUS_HEADER) {
                    GossipModuleHeader* header = GetDeserializedMsgHeader(status.recv_str);
                    int message_len = header->GetMessageLength();
                    if (message_len > 0) {
                        status.header       = header; // temp
                        status.message_len  = message_len;
                        status.recv_status  = STATUS_MSG;
                        status.received_len = 0;
                        status.recv_str     = "";
                    }
                    else {
                        std::cout << "[HandleRecvSocketIO] HEADER len error\n";
                    }
                }
                else if (status.recv_status == STATUS_MSG) {
                    status.recv_status = STATUS_IDLE;
                    Message* msg = GetDeserializedMsg(status.recv_str);
                    Membership_RunProtocol(peer->GetIP(), status.header, msg);
                }
            }
            break;
    }

    if (is_disconnect) {
        #if LOGGING
            std::cout << "[HandleRecvSocketIO] Disconnected with "<<peer->GetIP()<<'\n';
        #endif

        TimeoutManager.RemoveTimer(fd);
        socketEventPublisher.UnregisterDataSocket(fd);
        PeerMap.erase(it);
        std::string src = peer->GetIP();
        
		delete peer;
        close(fd);
        Membership_HandleDisconnect(src);
    }
    else {
        TimeoutManager.RestartTimer(fd);
    }
}

void HandleNetwork_GossipModel::onSendSocketReadyEvent(std::shared_ptr<EventInfo> info) {
    std::shared_ptr<SendSocketReadyEventInfo> derivedInfo = std::static_pointer_cast<SendSocketReadyEventInfo>(info);
    
	int fd = derivedInfo->GetSocketFD();
    auto it = PeerMap.find(fd);
    if (it == PeerMap.end()) {
        std::cout << "[onSendSocketReadyEvent] No valid peer exists\n";
        return;
    }

    Peer* peer = it->second;
    SocketSendStatus &status = sendStatusMap[fd];
    if (status.sendMsgQueue.empty()) {
        socketEventPublisher.UnsetSocketWrite(fd);
        return;
    }

    std::shared_ptr<SendMsg> msg = status.sendMsgQueue.front();
    int numbytes = send(fd, msg->dpos(), msg->nbytes(), 0);
    if (numbytes < 0) {
        Membership_HandleCrush(peer->GetIP());
        TimeoutManager.RemoveTimer(fd);
        socketEventPublisher.UnregisterDataSocket(fd);
        PeerMap.erase(it);
        delete peer;
        close(fd);
        return;;
    }

    msg->pos += numbytes;
    if (msg->nbytes() == 0) {
       status.sendMsgQueue.pop_front();
       //delete msg.get();
        #if LOGGING
            std::cout << "SEND\n";
            std::cout << "[onSendSocketReadyEvent] Send a Msg\n";
        #endif
   }
}

Peer* HandleNetwork_GossipModel::GetPeerFromPeerMap(std::string id) {
    for (auto &it : PeerMap) {
        Peer* peer = it.second;
        if (peer->GetIP() == id || peer->GetHostname() == id) return peer;
    }
    return NULL;
}

std::string HandleNetwork_GossipModel::GetSerializedMsgHeader(GossipModuleHeader* header) {
    std::string serial_str;
    boost::iostreams::back_insert_device<std::string> inserter(serial_str);
    boost::iostreams::stream<boost::iostreams::back_insert_device<std::string> > s(inserter);
    boost::archive::binary_oarchive oa(s);
    oa << header;
    s.flush();
    return serial_str;
}

GossipModuleHeader* HandleNetwork_GossipModel::GetDeserializedMsgHeader(std::string str) {
    GossipModuleHeader* header;
    boost::iostreams::basic_array_source<char> device(str.c_str(), str.size());
    boost::iostreams::stream<boost::iostreams::basic_array_source<char> > s(device);
    boost::archive::binary_iarchive ia(s);
    ia >> header;
    return header;
}

std::string HandleNetwork_GossipModel::GetSerializedMsg(Message* msg) {
    std::string serial_str;
    boost::iostreams::back_insert_device<std::string> inserter(serial_str);
    boost::iostreams::stream<boost::iostreams::back_insert_device<std::string> > s(inserter);
    boost::archive::binary_oarchive oa(s);
    oa << msg;
    s.flush();
    return serial_str;
}

Message* HandleNetwork_GossipModel::GetDeserializedMsg(std::string str) {
    Message* msg;
    boost::iostreams::basic_array_source<char> device(str.c_str(), str.size());
    boost::iostreams::stream<boost::iostreams::basic_array_source<char> > s(device);
    boost::archive::binary_iarchive ia(s);
    ia >> msg;
    return msg;
}

void HandleNetwork_GossipModel::Membership_HandleShutdownCallback(EV_P_ ev_timer *w, int revents) {
    std::cout << "[Membership_HandleShutdownCallback] called\n";
    HandleNetwork_GossipModel* instance = HandleNetwork_GossipModel::GetInstance();

    if (!isGossipShutdown) {
        std::vector<Neighbor*>& active  = instance->GetPartialViewManager()->GetActiveView();
        std::vector<Neighbor*>& passive = instance->GetPartialViewManager()->GetPassiveView();
        for (Neighbor* p : active)  delete p;
        for (Neighbor* p : passive) delete p;
        std::cout << "[NetworkGraph] "<< utility::GetCurrentTime() << " \n";

        std::map<int, Peer*>& pmap = *instance->GetPeerMap();
        for (auto& entry : pmap) {
            int fd = entry.first;
            instance->socketEventPublisher.UnregisterDataSocket(fd);
            close(fd);
            delete entry.second;
        }
        std::cout << "[Shutdown]\n";
        ev_loop_destroy(GlobalEvent::loop);
        exit(0); // End this node
    }
    else {
        std::vector<Neighbor*>& active = instance->GetPartialViewManager()->GetActiveView();
        for (Neighbor* p : active) {
            p->isEager = false;
        }
        //instance->GetPartialViewManager()->PrintActiveView();

        GossipModuleHeader newheader = GossipModuleHeader(CASTTYPE_BROADCAST, PROTOCOL_GOSSIP);
        newheader.SetSrcId(NodeInfo::GetInstance()->GetHostIP());
        newheader.IncreaseHopCount();
        GossipModuleMessage newmsg = GossipModuleMessage(GOSSIP_PRUNE);
        instance->Membership_BroadcastWithHeader("", &newheader, &newmsg);
    }
}

void HandleNetwork_GossipModel::Membership_HandleSendShuffleCallback(EV_P_ ev_timer *w, int revents) {
    HandleNetwork_GossipModel* instance = HandleNetwork_GossipModel::GetInstance();
    if (instance->GetPartialViewManager()->CurrentActiveViewSize() > 0) {
        #if LOGGING
            std::cout << "[Membership_HandleSendShuffleCallback] called\n";
        #endif
        Neighbor* target = instance->GetPartialViewManager()->ChooseRandomFromActive();

        GossipModuleHeader newheader(CASTTYPE_UNICAST, PROTOCOL_MEMBERSHIP);
        newheader.SetDstId(target->id);
        newheader.SetSrcId(NodeInfo::GetInstance()->GetHostIP());
        newheader.IncreaseHopCount();
        GossipModuleMessage newmsg(MEMBERSHIP_SHUFFLE);
        newmsg.SetShuffleList(instance->Membership_CreateShuffleList(Ka, Kp));
        instance->UnicastWithHeader(&newheader, &newmsg);
    }
    ev_timer_again(EV_A_ w);
}

// This function is called when the caller node thinks isolation is detected
void HandleNetwork_GossipModel::Membership_HandleIsolationDetectionCallback(EV_P_ ev_timer *w, int revents) {
    HandleNetwork_GossipModel* instance = HandleNetwork_GossipModel::GetInstance();

    int result = instance->Connect("bleep0");
    assert(result != -1);

    Peer* contactnode = instance->GetPeerFromPeerMap("bleep0");
    std::string contactnode_ip = contactnode->GetIP();

    GossipModuleHeader header(CASTTYPE_UNICAST, PROTOCOL_MEMBERSHIP);
    header.SetDstId(contactnode_ip);
    header.SetSrcId(NodeInfo::GetInstance()->GetHostIP());
    GossipModuleMessage msg(MEMBERSHIP_JOIN);
    instance->UnicastWithHeader(&header, &msg);

    instance->GetPartialViewManager()->AddToActive(contactnode_ip);
    std::cout << "[Membership_HandleIsolationDetectionCallback] Rejoin to Main Network\n";

    instance->GetPartialViewManager()->PrintActiveView();//    
    ev_timer_again(EV_A_ w);
}

void HandleNetwork_GossipModel::Membership_RegisterShutdownTimerWatcher() {
    if (shutdownTime <= 0) return;
    ev_timer_init(&Membership_ShutdownTimer, Membership_HandleShutdownCallback, shutdownTime, 0);
    ev_timer_start(GlobalEvent::loop, &Membership_ShutdownTimer);
    std::cout << "[Membership_RegisterShutdownTimerWatcher] register timer\n";
}

void HandleNetwork_GossipModel::Membership_RegisterShuffleTimerWatcher() {
    if (!SHUFFLE_OP) return;
    ev_timer_init(&Membership_CyclonTimer, Membership_HandleSendShuffleCallback, SHUFFLE_PERIOD, SHUFFLE_PERIOD);
    ev_timer_start(GlobalEvent::loop, &Membership_CyclonTimer);
    std::cout << "[Membership_RegisterShuffleTimerWatcher] register timer\n";
}

void HandleNetwork_GossipModel::Membership_RegisterIsolationTimerWatcher() {
    ev_timer_init(&Membership_IsolationTimer, Membership_HandleIsolationDetectionCallback, ISOLATION_CHECK_INTERVAL, ISOLATION_CHECK_INTERVAL);
    ev_timer_start(GlobalEvent::loop, &Membership_IsolationTimer);
    std::cout << "[Membership_RegisterIsolationTimerWatcher] register timer\n";
}

void HandleNetwork_GossipModel::Membership_RestartIsolationTimer() {
    ev_timer_stop(GlobalEvent::loop, &Membership_IsolationTimer);
    ev_timer_set(&Membership_IsolationTimer, ISOLATION_CHECK_INTERVAL, 0);
    ev_timer_start(GlobalEvent::loop, &Membership_IsolationTimer);
}


/*---------------------------------------------------------------------------------*/
void HandleNetwork_GossipModel::Eclipse_Attack_Callback(EV_P_ ev_timer *w, int revents) {
    HandleNetwork_GossipModel* instance = HandleNetwork_GossipModel::GetInstance();
    
    // If not connected, then make connection
    // Else, check target is active member or not
    Peer* target = instance->GetPeerFromPeerMap("bleep0");
    if (!target) { 
        int result = instance->Connect("bleep0");
        if (result == -1) {
            std::cout << "[Eclipse_Attack_Callback] Cannot connect to target\n";
            if (ECLIPSE_ITER) ev_timer_again(EV_A_ w);
            return;
        }
    }
    else {
        // If already connect & join with target, then do nothing
        Neighbor* ntarget = instance->GetPartialViewManager()->FindPeerFromActive(target->GetIP());
        if (ntarget) {    
            std::cout << "[Eclipse Attack_Callback] Already joined\n";
            if (ECLIPSE_ITER) ev_timer_again(EV_A_ w);
            return;
        }
    }

    // Get IP info
    target = instance->GetPeerFromPeerMap("bleep0");
    std::string target_ip = target->GetIP();

    if (instance->eclipseStart == 0) {
        instance->settarget(target_ip);
        instance->eclipseStart = 1;
        std::cout << "[Eclipse_Attack_Callback] attack start\n";  
    }    
    
    // Send neighbor with high priority
    GossipModuleHeader header(CASTTYPE_UNICAST, PROTOCOL_MEMBERSHIP);
    header.SetDstId(target_ip);
    header.SetSrcId(NodeInfo::GetInstance()->GetHostIP());
    GossipModuleMessage msg(MEMBERSHIP_NEIGHBOR);
    msg.SetOption(1);
    instance->UnicastWithHeader(&header, &msg);

    std::cout << "[Eclipse Attack_Callback] Send Neighbor with high priority\n";
    
    // Restart timer for periodic attack
    if (ECLIPSE_ITER) ev_timer_again(EV_A_ w);
}

void HandleNetwork_GossipModel::Eclipse_Attack_RegisterTimerWatcher() {
    if (eclipseTime<=0) return;
    ev_timer_init(&Eclipse_Attack_Timer, Eclipse_Attack_Callback, eclipseTime, ECLIPSE_INTERVAL);
    ev_timer_start(GlobalEvent::loop, &Eclipse_Attack_Timer);
    std::cout << "[Ecplise Attack] register timer\n";
}

/*---------------------------------------------------------------------------------*/

void HandleNetwork_GossipModel::Eclipse_Attack_Log_Callback(EV_P_ ev_timer *w, int revents) {
    HandleNetwork_GossipModel* instance = HandleNetwork_GossipModel::GetInstance();
    
    instance->GetPartialViewManager()->PrintPartialView();
    ev_timer_again(EV_A_ w);
}

void HandleNetwork_GossipModel::Eclipse_Attack_Log_RegisterTimerWatcher() {
    if (eclipseLogTime<=0) return;
    ev_timer_init(&Eclipse_Attack_Log_Timer, Eclipse_Attack_Log_Callback, eclipseLogTime, 0.1);
    ev_timer_start(GlobalEvent::loop, &Eclipse_Attack_Log_Timer);
    std::cout << "[Ecplise Attack] register log timer\n";
}

/*---------------------------------------------------------------------------------*/

int HandleNetwork_GossipModel::Membership_ProcessShuffleList(std::vector<std::string> list) {
    int cnt = 0;
    std::string hostid = NodeInfo::GetInstance()->GetHostIP();
    for (int i = 0; i<list.size(); i++) {
        std::string newid = list[i];
        if (hostid == newid) continue;
        PartialViewManager.AddToPassive(newid);
        cnt += 1;
    }
    PartialViewManager.PrintPartialView();//
    return cnt;
}

std::vector<std::string> HandleNetwork_GossipModel::Membership_CreateShuffleList(int na, int np) {
    int a_size = PartialViewManager.CurrentActiveViewSize();
    int p_size = PartialViewManager.CurrentPassiveViewSize();

    int num_a = (na >= a_size) ? a_size : na;
    int num_p = (np >= p_size) ? p_size : np;

    std::vector<std::string> random_active  = PartialViewManager.GetRandomActiveMemberList(num_a);
    std::vector<std::string> random_passive = PartialViewManager.GetRandomPassiveMemberList(num_p);

    std::vector<std::string> list;
    list.emplace_back(NodeInfo::GetInstance()->GetHostIP());
    list.insert(list.end(), random_active.begin(),  random_active.end());
    list.insert(list.end(), random_passive.begin(), random_passive.end());
    return list;
}

int HandleNetwork_GossipModel::JoinNetwork() {
    std::cout << "StartTime "<< utility::GetCurrentTime() <<'\n';

    int listenfd = InitializeListenSocket();
    socketEventPublisher.RegisterSocketAsServerSocket(listenfd);
    Membership_RegisterShutdownTimerWatcher();
    Membership_RegisterShuffleTimerWatcher();

    /* Eclipse Attack */
    Eclipse_Attack_RegisterTimerWatcher();
    Eclipse_Attack_Log_RegisterTimerWatcher();
    
    if (amIContactNode) {
        std::cout << "[JoinNetwork] NodeType CONTACT\n";
        return 0;
    }
    if (eclipseTime > 0) {
        std::cout << "[JoinNetwork] NodeType ECLIPSE\n";
    }

    int result = Connect(contact);
    if (amIFullNode) {
        Membership_RegisterIsolationTimerWatcher();

        Peer* contactnode = GetPeerFromPeerMap(contact);
        std::string contactnode_ip = contactnode->GetIP();

        GossipModuleHeader header(CASTTYPE_UNICAST, PROTOCOL_MEMBERSHIP);
        header.SetDstId(contactnode_ip);
        header.SetSrcId(NodeInfo::GetInstance()->GetHostIP());
        GossipModuleMessage msg(MEMBERSHIP_JOIN);
        UnicastWithHeader(&header, &msg);

        PartialViewManager.AddToActive(contactnode_ip);
        PartialViewManager.PrintActiveView();//
        std::cout << "[JoinNetwork] NodeType FULL\n";
    }
    else {
        std::cout << "[JoinNetwork] NodeType INJECTOR\n";
    }
    return result;
}

int HandleNetwork_GossipModel::Connect(std::string targetid) {
    if (GetPeerFromPeerMap(targetid)) {
        std::cout << "[Connect] Connection with target already exists\n";
        return 0;
    }

    struct addrinfo* peerinfo;
    int res = getaddrinfo((const char*)targetid.c_str(), NULL, NULL, &peerinfo);
    if (res == -1) return -1;

    int sfd = socket(AF_INET, (SOCK_STREAM | SOCK_NONBLOCK), 0);
    if (sfd == -1) {
        freeaddrinfo(peerinfo);
        return -1;
    }

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family      = AF_INET;
    address.sin_port        = htons(MYPORT);
    address.sin_addr.s_addr = ((struct sockaddr_in*)(peerinfo->ai_addr))->sin_addr.s_addr;

    res = connect(sfd, (struct sockaddr*)&address, sizeof(address));
    if (res == -1 && errno != EINPROGRESS) {
        freeaddrinfo(peerinfo);
        close(sfd);
        return -1;
    }

    Peer* peer = new Peer(sfd, targetid, std::string(inet_ntoa(address.sin_addr)));
    PeerMap[sfd] = peer;
    socketEventPublisher.RegisterSocketAsDataSocket(sfd);
    TimeoutManager.RegisterTimeoutWatcher(sfd);
    return 0;
}

int HandleNetwork_GossipModel::UnicastWithHeader(GossipModuleHeader* header, Message* msg) {
    std::string destip = header->GetDstId();
    Peer* peer = GetPeerFromPeerMap(destip);
    if (!peer) {
        std::cout << "[UnicastWithHeader] no peer object exists for " << destip << '\n';
        return -1;
    }

    std::string serialized_message = GetSerializedMsg(msg);
    int message_len = serialized_message.size();
    header->SetMessageLength(message_len);

    std::string serialized_header = GetSerializedMsgHeader(header);
    int header_len = serialized_header.size();

    int fd = peer->GetSocketFD();
    SocketSendStatus& status = sendStatusMap[fd];
    status.sendMsgQueue.push_back(std::shared_ptr<SendMsg>(new SendMsg((char*)&header_len, sizeof(int))));
    status.sendMsgQueue.push_back(std::shared_ptr<SendMsg>(new SendMsg(serialized_header.c_str(), header_len)));
    status.sendMsgQueue.push_back(std::shared_ptr<SendMsg>(new SendMsg(serialized_message.c_str(), message_len)));

    socketEventPublisher.SetSocketWrite(fd);
    return 0;
}

void HandleNetwork_GossipModel::UnicastMsg(std::string destip, Message* msg) {
    Peer* peer = GetPeerFromPeerMap(destip);
    if (!peer) {
        if (Connect(destip) == -1) return;
    }
    GossipModuleHeader header(CASTTYPE_UNICAST, PROTOCOL_CONSENSUS);
    header.SetDstId(destip);
    header.SetSrcId(NodeInfo::GetInstance()->GetHostIP());
    UnicastWithHeader(&header, msg);
}

void HandleNetwork_GossipModel::BroadcastMsg(Message* msg) {
    // Need to decide Injecting Policy
    // separate injector not full node vs full node as injector (after stabilization)
    if (!amIFullNode) {
        Peer* contactnode = GetPeerFromPeerMap(contact);
        if (!contactnode) return;

        size_t mid = Gossip_MessageManager.CreateNewId(msg);
        std::string contactnode_ip = contactnode->GetIP();

        GossipModuleHeader header(CASTTYPE_BROADCAST, PROTOCOL_CONSENSUS);
        header.SetDstId(contactnode_ip);
        header.SetSrcId(NodeInfo::GetInstance()->GetHostIP());
        header.SetMessageId(mid);
        header.SetIsInjected(true);
        UnicastWithHeader(&header, msg);

        std::cout<<"[INJECTOR] "<<utility::GetCurrentTime()<<std::setw(21)<<mid<<' '<<contactnode_ip<<'\n';
        return;
    }

    size_t mid = Gossip_MessageManager.CreateNewId(msg);
    Gossip_MessageManager.AddToStorage(mid, 0, NodeInfo::GetInstance()->GetHostIP(), msg);

    GossipModuleHeader header(CASTTYPE_BROADCAST, PROTOCOL_CONSENSUS);
    header.SetSrcId(NodeInfo::GetInstance()->GetHostIP());
    header.SetMessageId(mid);
    Gossip_BroadcastWithHeader("", &header, msg, true);
}

void HandleNetwork_GossipModel::Membership_BroadcastWithHeader(std::string exception, GossipModuleHeader* header, Message* msg) {
    std::vector<Neighbor*>& activeview = PartialViewManager.GetActiveView();
    for (Neighbor* p : activeview) {
        if (p->id == exception) continue;
        header->SetDstId(p->id);
        UnicastWithHeader(header, msg);
    }
}

void HandleNetwork_GossipModel::Membership_SendNeighbor(std::string exception) {
    Neighbor* candidate = NULL;
    while (1) {
        candidate = PartialViewManager.ChooseRandomFromPassiveExcept(exception);
        if (!candidate) return;

        std::string id = candidate->id;
        Peer* peer = GetPeerFromPeerMap(id);
        if (!peer) {
            #if LOGGING
                std::cout << "[Membership_SendNeighbor] have no connection\n";
            #endif
            if (Connect(id) == -1) {
                PartialViewManager.DropFromPassive(id);
                PartialViewManager.PrintPartialView();//
                continue;
            }
            #if LOGGING
                std::cout << "[Membership_SendNeighbor] make connection\n";
            #endif
        }
        else {
            #if LOGGING
                std::cout << "[Membership_SendNeighbor] have connection already\n";
            #endif
        }
        break;
    }

    int is_highpriority = (PartialViewManager.CurrentActiveViewSize() == 0) ? 1:0;
    GossipModuleHeader header(CASTTYPE_UNICAST, PROTOCOL_MEMBERSHIP);
    header.SetDstId(candidate->id);
    header.SetSrcId(NodeInfo::GetInstance()->GetHostIP());
    GossipModuleMessage msg(MEMBERSHIP_NEIGHBOR);
    msg.SetOption(is_highpriority);
    UnicastWithHeader(&header, &msg);
}

void HandleNetwork_GossipModel::Membership_HandleDisconnect(std::string src) {
    if (PartialViewManager.FindPeerFromActive(src)) {
        std::cout << "[Membership_HandleDisconnect] active-timeout with "<<src<<"\n";
        PartialViewManager.DropFromActive(src);
        PartialViewManager.AddToPassive(src);
        Membership_SendNeighbor("");
        PartialViewManager.PrintActiveView();//
    }
}

void HandleNetwork_GossipModel::Membership_HandleCrush(std::string src) {
    std::cout << "[Membership_HandleCrush] called with "<<src<<"\n";
    if (PartialViewManager.DropFromActive(src)) {
        Membership_SendNeighbor("");
    }
    else {
        PartialViewManager.DropFromPassive(src);
    }
    PartialViewManager.PrintActiveView();//
}

void HandleNetwork_GossipModel::Membership_RunProtocol(std::string sender,GossipModuleHeader* header,Message* msg) {
    int ptype = header->GetProtocolType();

    // target type = PROTOCOL_MEMBERSHIP
    // msg that has this type is used for maintaining neighborhood membership
    // neighborhood membership is foundation of gossipping
    if (ptype == PROTOCOL_MEMBERSHIP) {
        GossipModuleMessage* gmsg = dynamic_cast<GossipModuleMessage*>(msg);
        assert(gmsg);

        std::string src = header->GetSrcId();
        int hop  = header->GetHopCount();
        int opt  = (int)gmsg->GetOption();
        int type = gmsg->GetMessageType();

        switch(type) {
            case MEMBERSHIP_JOIN:
                {
                    bool res = PartialViewManager.AddToActive(src);
                    PartialViewManager.PrintActiveView();//
                    
                    if (TEST_OP) break;
                    if (res) {
                        GossipModuleHeader newheader(CASTTYPE_BROADCAST, PROTOCOL_MEMBERSHIP);
                        newheader.SetSrcId(src);
                        newheader.IncreaseHopCount();
                        GossipModuleMessage newmsg(MEMBERSHIP_FORWARDJOIN);
                        Membership_BroadcastWithHeader(src, &newheader, &newmsg);
                    }
                }
                break;

            case MEMBERSHIP_FORWARDJOIN:
                {
                    bool is_exist = false;
                    if (hop == ARWL || PartialViewManager.CurrentActiveViewSize() <= 1) {
                        if (!PartialViewManager.FindPeerFromActive(src)) {
                            PartialViewManager.AddToActive(src);
                            PartialViewManager.PrintActiveView();//                            
                            Peer* peer = GetPeerFromPeerMap(src);
                            if (!peer) {
                                if (Connect(src) == -1) {
                                    Membership_HandleCrush(src);
                                    break;
                                }
                            }

                            GossipModuleHeader newheader(CASTTYPE_UNICAST, PROTOCOL_MEMBERSHIP);
                            newheader.SetDstId(src);
                            newheader.SetSrcId(NodeInfo::GetInstance()->GetHostIP());
                            newheader.IncreaseHopCount();
                            GossipModuleMessage newmsg(MEMBERSHIP_FORWARDJOINREPLY);
                            UnicastWithHeader(&newheader, &newmsg);
                            break;
                        }
                        else {
                            is_exist = true;
                        }
                    }
                    if (hop == PRWL) {
                        PartialViewManager.AddToPassive(src);
                        PartialViewManager.PrintPartialView();
                    }

                    Neighbor* candidate = PartialViewManager.ChooseRandomFromActiveExcept(src);
                    if (candidate) {
                        header->SetDstId(candidate->id);
                        if (!is_exist) header->IncreaseHopCount();
                        UnicastWithHeader(header, msg);
                    }
                }
                break;

            case MEMBERSHIP_FORWARDJOINREPLY:
                {
                    PartialViewManager.AddToActive(src);
                    PartialViewManager.PrintActiveView();//
                }
                break;

            case MEMBERSHIP_DISCONNECT:
                {
                    // Eclipse Node does nothig.
                    if (eclipseTime > 0) {
                        if (eclipseStart) {
                            if (PartialViewManager.FindPeerFromActive(src)) {
                                PartialViewManager.DropFromActive(src);
                                PartialViewManager.AddToPassive(src);
                                PartialViewManager.PrintActiveView();//
                            }                          
                            break;
                        }
                    }
                    
                    if (PartialViewManager.FindPeerFromActive(src)) {
                        PartialViewManager.DropFromActive(src);
                        PartialViewManager.AddToPassive(src);
                        Membership_SendNeighbor("");
                        PartialViewManager.PrintActiveView();//
                    }
                }
                break;

            case MEMBERSHIP_NEIGHBOR:
                {
                    if (PartialViewManager.FindPeerFromActive(src)) break;

                    int is_accept = 1;
                    if (!opt && PartialViewManager.CurrentActiveViewSize() == ActiveViewSize) {
                        is_accept = 0;
                    }
                    if (is_accept) {
                        bool res = PartialViewManager.AddToActive(src);                        
                        if (res) PartialViewManager.PrintActiveView();//
                    }

                    GossipModuleHeader header(CASTTYPE_UNICAST, PROTOCOL_MEMBERSHIP);
                    header.SetDstId(src);
                    header.SetSrcId(NodeInfo::GetInstance()->GetHostIP());
                    GossipModuleMessage msg(MEMBERSHIP_NEIGHBORREPLY);
                    msg.SetOption(is_accept);
                    UnicastWithHeader(&header, &msg);
                }
                break;

            case MEMBERSHIP_NEIGHBORREPLY:
                {
                    if (PartialViewManager.FindPeerFromActive(src)) break;
                    if (opt) {
                        PartialViewManager.AddToActive(src);
                        PartialViewManager.PrintActiveView();//
                        break;
                    }
                    if (PartialViewManager.CurrentActiveViewSize() < ActiveViewSize) {
                        Membership_SendNeighbor(src);
                    }
                }
                break;

            case MEMBERSHIP_SHUFFLE:
                {
                    // Eclipse Node does nothig.
                    if (eclipseTime > 0) {
                        if (eclipseStart) {
                            // isolate target node by dropping hello msgs.
                            break;
                        }
                    }

                    if (hop == SRWL || PartialViewManager.CurrentActiveViewSize() <= 1) {
                        int cnt = Membership_ProcessShuffleList(gmsg->GetShuffleList());

                        Peer* peer = GetPeerFromPeerMap(src);
                        if (!peer) {
                            if (Connect(src) == -1) {
                                Membership_HandleCrush(src);
                                break;
                            }
                        }
                        GossipModuleHeader newheader(CASTTYPE_UNICAST, PROTOCOL_MEMBERSHIP);
                        newheader.SetDstId(src);
                        newheader.SetSrcId(NodeInfo::GetInstance()->GetHostIP());
                        newheader.IncreaseHopCount();
                        GossipModuleMessage newmsg(MEMBERSHIP_SHUFFLEREPLY);
                        newmsg.SetShuffleList(Membership_CreateShuffleList(0, cnt-1));
                        UnicastWithHeader(&newheader, &newmsg);
                    }

                    if (src == "bleep0") {
                        Membership_RestartIsolationTimer();
                    }
                    if (hop >= 15) {
                        break;
                    }

                    Neighbor* candidate = PartialViewManager.ChooseRandomFromActiveExcept(src);
                    if (candidate) {
                        header->SetDstId(candidate->id);
                        header->IncreaseHopCount();
                        UnicastWithHeader(header, msg);
                    }
                }
                break;

            case MEMBERSHIP_SHUFFLEREPLY:
                {
                    Membership_ProcessShuffleList(gmsg->GetShuffleList());
                }
                break;

            default:
                break;
        }

        delete header;
        delete msg;
        return;
    }

    // target type = PROTOCOL_CONSENSUS(CASTTYPE_UNICAST)
    // Unicasted CONSENSUS msg is directly passed to CONSENSUS state
    // it does not need to pass GOSSIP state
    if (ptype == PROTOCOL_CONSENSUS) {
        int ttype = header->GetTransferType();
        if (ttype == CASTTYPE_UNICAST) {
            if (msg->GetType() == Message::CONSENSUS_MESSAGE) {
                std::cout << "[Membership_RunProtocol] UNICAST CONSENSUS-MESSAGE\n";

                //ConsensusMessage *conmsg = dynamic_cast<ConsensusMessage*>(msg);
                //handleConsensusClass->HandleArrivedConsensusMsg(conmsg);
                delete header;
                delete msg;
            }
            else {
                std::cout << "[Membership_RunProtocol] UNICAST NON CONSENSUS-MESSAGE\n";
            }
            return;
        }
    }

    // target type = PROTOCOL_CONSENSUS(CASTTYPE_BROADCAST), PROTOCOL_GOSSIP
    // call Gossip Protocol handling function, RunGossipProtocol
    // For CONSENSUS msg, toss it to CONSENSUS state
    // Test_Gossip_Runtime is a basic gossip function used at verifying membership functionalities
    if (TEST_MEMBERSHIP) {
        Test_Gossip_RunProtocol(sender, header, msg);
    }
    else {
        Gossip_RunProtocol(sender, header, msg);
    }
}

void HandleNetwork_GossipModel::Gossip_BroadcastWithHeader(std::string exception, GossipModuleHeader* header, Message* msg, bool isEager) {
    std::vector<Neighbor*>& activeview = PartialViewManager.GetActiveView();
    for (Neighbor* p : activeview) {
        if (p->id == exception) continue;
        if (p->isEager == isEager) {
            header->SetDstId(p->id);
            UnicastWithHeader(header, msg);
        }
    }
}

void HandleNetwork_GossipModel::Gossip_LazyPush(size_t mid, int round) {
    GossipModuleHeader newheader(CASTTYPE_BROADCAST, PROTOCOL_GOSSIP);
    newheader.SetSrcId(NodeInfo::GetInstance()->GetHostIP());
    newheader.IncreaseHopCount();
    GossipModuleMessage newmsg(GOSSIP_IHAVE);
    newmsg.SetOption(mid);
    Gossip_BroadcastWithHeader("", &newheader, &newmsg, false);
}

void HandleNetwork_GossipModel::Gossip_HandleCrush(std::string src) {
    //remove all timers related to src
    ;
}

void HandleNetwork_GossipModel::Gossip_RunProtocol(std::string sender, GossipModuleHeader* header, Message* msg) {
    int ptype = header->GetProtocolType();
    
    // size
    //std::cout<<"[SIZE] "<<utility::GetCurrentTime()<<" "<<sizeof(header)<<" "<<sizeof(msg)<<" "<<sizeof(header)+sizeof(msg)<<'\n';
    
    // treat PRUNE, IHAVE, GRAFT
    if (ptype == PROTOCOL_GOSSIP) {
        GossipModuleMessage* gmsg = dynamic_cast<GossipModuleMessage*>(msg);
        assert(gmsg);

        switch(gmsg->GetMessageType()) {
            case GOSSIP_PRUNE:
            {
                //std::cout << "PLUMTREE "<<utility::GetCurrentTime()<<" PRUNE\n";
                Neighbor* neighbor = PartialViewManager.FindPeerFromActive(sender);
                if (!neighbor) {
                    std::cout << "[Gossip_RunProtocol] PRUNE No Active link exists for " << sender << '\n';
                    break;
                }
                if (neighbor->isEager == true) {
                    neighbor->isEager = false;
                    PartialViewManager.PrintActiveView();
                }
            }
            break;

            case GOSSIP_IHAVE:
            {
                //std::cout << "PLUMTREE "<<utility::GetCurrentTime()<<" IHAVE\n";
                size_t mid = gmsg->GetOption();
                bool is_msg_exist = Gossip_MessageManager.IsExist(mid);
                if (!is_msg_exist) {
                    Gossip_RecoveryManager.AddToStorage(mid, header->GetHopCount(), sender);
                }
            }
            break;

            case GOSSIP_GRAFT:
            {
                //std::cout << "PLUMTREE "<<utility::GetCurrentTime()<<" GRAFT\n";
                Neighbor* neighbor = PartialViewManager.FindPeerFromActive(sender);
                if (!neighbor) {
                    std::cout << "[Gossip_RunProtocol] GRAFT No Active link exists for " << sender << '\n';
                    break;
                }
                if (neighbor->isEager == false) {
                    neighbor->isEager = true;
                    PartialViewManager.PrintActiveView();
                }

                size_t mid = gmsg->GetOption();
                IHAVE_MESSAGE* entry = Gossip_MessageManager.GetMessageEntry(mid);
                assert(entry);

                GossipModuleHeader newheader = GossipModuleHeader(CASTTYPE_BROADCAST, PROTOCOL_CONSENSUS);
                newheader.SetDstId(sender);
                newheader.SetSrcId(NodeInfo::GetInstance()->GetHostIP());
                newheader.SetHopCount(entry->round + 1);
                newheader.SetMessageId(mid);
                Message* stored_msg = entry->msg;
                UnicastWithHeader(&newheader, stored_msg);
            }
            break;
        }
        return;
    }

    // Now, treat PROTOCOL_CONSENSUS as GOSSIP_GOSSSIP type
    if (ptype == PROTOCOL_CONSENSUS) {
        size_t mid = header->GetMessageId();
        bool is_msg_exist = Gossip_MessageManager.IsExist(mid);

        Neighbor* neighbor = PartialViewManager.FindPeerFromActive(sender);
        if (!neighbor) {
            if (header->IsInjected()) {
                std::cout<<"NEWMSG "<<"NEW "<< utility::GetCurrentTime()<<' '<<mid<<' '<<PartialViewManager.FanoutSize()<<'\n';

                Gossip_MessageManager.AddToStorage(mid, header->GetHopCount(), NodeInfo::GetInstance()->GetHostIP() , msg);
                header->IncreaseHopCount();
                header->SetIsInjected(false);
                Gossip_BroadcastWithHeader("", header, msg, true);
                Gossip_LazyPush(mid, header->GetHopCount()-1);
            }
            else {
                std::cout << "[Gossip_RunProtocol] No link exists for " << sender << '\n';
                return;
            }
        }
        else {
            if (neighbor->isEager == true) {
                if (is_msg_exist) {
                    std::cout<<"NEWMSG "<<"DUP "<< utility::GetCurrentTime()<<' '<<mid<<' '<<PartialViewManager.FanoutSize()<<'\n';
  
                    neighbor->isEager = false;
                    PartialViewManager.PrintActiveView();

                    GossipModuleHeader newheader(CASTTYPE_UNICAST, PROTOCOL_GOSSIP);
                    newheader.SetDstId(sender);
                    newheader.SetSrcId(NodeInfo::GetInstance()->GetHostIP());
                    newheader.IncreaseHopCount();
                    GossipModuleMessage newmsg(GOSSIP_PRUNE);
                    UnicastWithHeader(&newheader, &newmsg);
                    return;
                }

                RecoveryEntry* entry = Gossip_RecoveryManager.FindFirstRecoveryEntry(mid);
                if (entry) {
                    if (sender != entry->sender) {
                        if (entry->timer_phase >= 2) return;
                    }
                    Gossip_RecoveryManager.StopTimerAndRemoveEntries(mid);
                }

                std::cout<<"NEWMSG "<<"NEW "<< utility::GetCurrentTime()<<' '<<mid<<' '<<PartialViewManager.FanoutSize()<<'\n';

                Gossip_MessageManager.AddToStorage(mid, header->GetHopCount(), sender, msg);
                header->IncreaseHopCount();
                Gossip_BroadcastWithHeader(sender, header, msg, true);
                Gossip_LazyPush(mid, header->GetHopCount()-1);
            }
            else {
                if (is_msg_exist) {
                    std::cout<<"NEWMSG "<<"DUP "<< utility::GetCurrentTime()<<' '<<mid<<' '<<PartialViewManager.FanoutSize()<<'\n';
                    return;
                }

                RecoveryEntry* entry = Gossip_RecoveryManager.FindFirstRecoveryEntry(mid);
                if (!entry) {
                    std::cout << "[Gossip_RunProtocol] LAZY new msg with NO timer from "<<sender<<"\n";
                    return;
                }
                if (sender != entry->sender) {
                    std::cout << "[Gossip_RunProtocol] LAZY new msg with timer from anoter peer,"<<sender<<"\n";
                    return;
                }
                if (entry->timer_phase < 2) {
                    std::cout << "[Gossip_RunProtocol] LAZY new msg at invalid timer phase\n";
                    return;
                }

                std::cout<<"NEWMSG "<<"NEW "<< utility::GetCurrentTime()<<' '<<mid<<' '<<PartialViewManager.FanoutSize()<<'\n';
                
                Gossip_RecoveryManager.StopTimerAndRemoveEntries(mid);
                neighbor->isEager = true;
                PartialViewManager.PrintActiveView();

                Gossip_MessageManager.AddToStorage(mid, header->GetHopCount(), sender, msg);
                header->IncreaseHopCount();
                Gossip_BroadcastWithHeader(sender, header, msg, true);
                Gossip_LazyPush(mid, header->GetHopCount()-1);
            }
        }
    }

    switch(msg->GetType()) {
        case Message::TEST_MESSAGE:
        {
            delete header;
        }
        break;

        case Message::TX_MESSAGE:
        {
            //Transaction *tx = dynamic_cast<Transaction*>(msg);
            //boost::shared_ptr<Transaction> shared_tx(tx);
            //handleTransactionClass->HandleArrivedTx(shared_tx);
            delete header;
        }
        break;

        case Message::CONSENSUS_MESSAGE:
        {
            //ConsensusMessage *conmsg = dynamic_cast<ConsensusMessage*>(msg);
            //handleConsensusClass->HandleArrivedConsensusMsg(conmsg);
            delete header;
        }
        break;

        default:
        break;
    }
}

// Test_Gossip_Protocol is simple flooding protocol for membership reliability testing
void HandleNetwork_GossipModel::Test_Gossip_RunProtocol(std::string sender, GossipModuleHeader* header, Message* msg) {
    int ptype = header->GetProtocolType();

    // Treat PROTOCOL_CONSENSUS as GOSSIP_GOSSSIP type + this is only the type used for testing
    if (ptype == PROTOCOL_CONSENSUS) {
        size_t mid = header->GetMessageId();
        bool is_msg_exist = Gossip_MessageManager.IsExist(mid);

        // Gossip received msg once it receives the msg first time
        // So, only the important is mid used to decide whether it is first time or not
        // Don't need to store sender inside Storage
        if (is_msg_exist) return;
        Gossip_MessageManager.AddToStorage(mid, header->GetHopCount(), "" , msg);

        if (header->IsInjected()) {
            header->SetIsInjected(false);
        }
        header->IncreaseHopCount();
        Gossip_BroadcastWithHeader("", header, msg, true);
    }
    else {
        std::cout << "[Test_Gossip_RunProtocol] Invalid Message Type\n";
    }
}

/* ------ Functions related to Connection-Timeout operation ----- */
TimeoutEntry* TimeoutList::FindTimeoutEntry(int sfd) {
    for (int i = 0; i<timeoutlist.size(); i++) {
        if (timeoutlist[i]->sfd == sfd) return timeoutlist[i];
    }
    return NULL;
}

void TimeoutList::RegisterTimeoutWatcher(int sfd) {
    TimeoutEntry* entry = new TimeoutEntry(sfd);
    ev_timer* timer = &entry->timeout_timer;
    timer->data = entry;
    timeoutlist.push_back(entry);

    ev_timer_init(timer, ConnectionTimeoutCallback, CONNECTION_TIMEOUT, 0);
    ev_timer_start(GlobalEvent::loop, timer);
}

void TimeoutList::RestartTimer(int sfd) {
    TimeoutEntry* entry = FindTimeoutEntry(sfd);
    assert(entry);

    ev_timer* timer = &entry->timeout_timer;
    ev_timer_stop(GlobalEvent::loop, timer);
    ev_timer_set(timer, CONNECTION_TIMEOUT, 0);
    ev_timer_start(GlobalEvent::loop, timer);
}

void TimeoutList::RemoveTimer(int sfd) {
    TimeoutEntry* entry = NULL;

    for (int i=0; i<timeoutlist.size(); i++) {
        entry = timeoutlist[i];
        if (entry->sfd == sfd) {
            ev_timer_stop(GlobalEvent::loop, &entry->timeout_timer);
            timeoutlist.erase(timeoutlist.begin()+i);
            delete entry;
            return;
        }
    }
}

void TimeoutList::ConnectionTimeoutCallback(EV_P_ ev_timer *w, int revents) {
    HandleNetwork_GossipModel* instance = HandleNetwork_GossipModel::GetInstance();
    TimeoutEntry* entry = (TimeoutEntry*)w->data;
    int sfd = entry->sfd;

    std::map< int, Peer* >* PeerMap = instance->GetPeerMap();
    auto peer_it = PeerMap->find(sfd);
    if (peer_it == PeerMap->end()) {
        std::cout << "[ConnectionTimeoutCallback] No Peer exists\n";
        exit(-1);
    }

    #if LOGGING
        std::cout << "[ConnectionTimeoutCallback] Timeout occurs\n";
    #endif
    Peer* peer = peer_it->second;
    if (instance->GetPartialViewManager()->FindPeerFromActive(peer->GetIP())) {
        ev_timer* timer = &entry->timeout_timer;
        ev_timer_stop(GlobalEvent::loop, timer);
        ev_timer_set(timer, CONNECTION_TIMEOUT, 0);
        ev_timer_start(GlobalEvent::loop, timer);
        return;
    }

    std::string src = peer->GetIP();
    instance->GetTimeoutManager()->RemoveTimer(sfd);
    instance->socketEventPublisher.UnregisterDataSocket(sfd);
    PeerMap->erase(peer_it);
    delete peer;
    close(sfd);

    instance->Membership_HandleDisconnect(src);
}

/* ------ Functions related to PartialView operation ----- */
bool PartialView::AddToActive(std::string id) {
    DropFromPassive(id);
    if (FindPeerFromActive(id)) return false;
    if (CurrentActiveViewSize() == ActiveViewSize) {
        DropRandomFromActive();
    }
    Neighbor* newnode = new Neighbor(id);
    active.push_back(newnode);
    PrintActiveView();
    return true;
}

bool PartialView::DropFromActive(std::string id) {
    for (int i=0; i<active.size(); i++) {
        Neighbor* temp = active[i];
        if (temp->id == id) {
            active.erase(active.begin()+i);
            delete temp;
            PrintActiveView();
            return true;
        }
    }
    return false;
}
void PartialView::DropRandomFromActive() {
    srand(time(0));
    int idx = rand() % (int)active.size();
    Neighbor* victim = active[idx];
    active.erase(active.begin() + idx);
    //std::cout << "[RANDOM_A] "<<idx << "\n";
    PrintActiveView();
    AddToPassive(victim->id);

    GossipModuleHeader header(CASTTYPE_UNICAST, PROTOCOL_MEMBERSHIP);
    header.SetDstId(victim->id);
    header.SetSrcId(NodeInfo::GetInstance()->GetHostIP());
    GossipModuleMessage msg(MEMBERSHIP_DISCONNECT);
    HandleNetwork_GossipModel::GetInstance()->UnicastWithHeader(&header, &msg);

    delete victim;
}

Neighbor* PartialView::FindPeerFromActive(std::string id) {
    for (int i=0; i<active.size(); i++){
        if (active[i]->id == id) return active[i];
    }
    return NULL;
}

Neighbor* PartialView::ChooseRandomFromActive() {
    if (active.size() == 0) return NULL;
    srand(time(0));
    int idx = rand() % (int)active.size();
    return active[idx];
}

Neighbor* PartialView::ChooseRandomFromActiveExcept(std::string id) {
    int size = active.size();
    if (size == 0) return NULL;
    if (size == 1 && active[0]->id == id) return NULL;
    else {
        int idx = rand() % size;
        if (active[idx]->id == id) return active[(idx+1) % size];
        else return active[idx];
    }
}

void PartialView::AddToPassive(std::string id) {
    if (FindPeerFromActive(id))  return;
    if (FindPeerFromPassive(id)) return;
    if (CurrentPassiveViewSize() == PassiveViewSize) {
        DropRandomFromPassive();
    }
    Neighbor* newnode = new Neighbor(id);
    passive.push_back(newnode);
    PrintPassiveView();
}

bool PartialView::DropFromPassive(std::string id) {
    for (int i=0; i<passive.size(); i++) {
        Neighbor* temp = passive[i];
        if (temp->id == id) {
            passive.erase(passive.begin()+i);
            delete temp;
            PrintPassiveView();
            return true;
        }
    }
    return false;
}
void PartialView::DropRandomFromPassive() {
    srand(time(0));
    int idx = rand() % (int)passive.size();
    //std::cout << "[RANDOM_P] "<<idx << "\n";
    passive.erase(passive.begin() + idx);
    PrintPassiveView();
}

Neighbor* PartialView::FindPeerFromPassive(std::string id) {
    for (int i=0; i<passive.size(); i++){
        if (passive[i]->id == id) return passive[i];
    }
    return NULL;
}

Neighbor* PartialView::ChooseRandomFromPassive() {
    if (passive.size() == 0) return NULL;

    srand(time(0));
    int idx = rand() % (int)passive.size();
    std::cout << idx << "\n";
    return passive[idx];
}

Neighbor* PartialView::ChooseRandomFromPassiveExcept(std::string id) {
    int size = (int)passive.size();
    if (size == 0) return NULL;
    if (size == 1 && passive[0]->id == id) return NULL;
    else {
        int idx = rand() % size;
        if (passive[idx]->id == id) return passive[(idx+1) % size];
        else return passive[idx];
    }
}

std::vector<std::string> PartialView::GetRandomActiveMemberList(int n) {
    std::vector<std::string> list;
    for (int i = 0; i < n; i++)
        list.emplace_back(active[i]->id);
    return list;
}

std::vector<std::string> PartialView::GetRandomPassiveMemberList(int n) {
    std::vector<std::string> list;
    for (int i = 0; i < n; i++)
        list.emplace_back(passive[i]->id);
    return list;
}

int PartialView::FanoutSize() {
    int cnt = 0;
    for (int i=0; i<active.size(); i++) {
        if (active[i]->isEager) cnt++;
    }
    return cnt;
}

void PartialView::PrintActiveView() {
    std::cout << "[NetworkGraph] "<< utility::GetCurrentTime() << " ";
    if (active.size() == 0) {
        std::cout << '\n';
        return;
    }
    for (int i=0; i<active.size(); i++) {
        std::cout << active[i]->id <<"("<< active[i]->isEager <<")";
        if (i == active.size()-1)
            std::cout << '\n';
        else
            std::cout << ',';
    }
    PrintPartialView();
}
void PartialView::PrintPassiveView() {
    return;
    std::cout << "[PassiveView] "<< utility::GetCurrentTime() << " ";
    if (passive.size() == 0) {
        std::cout << '\n';
        return;
    }
    for (int i=0; i<passive.size(); i++) {
        std::cout << passive[i]->id;
        if (i == passive.size()-1)
            std::cout << '\n';
        else
            std::cout << ',';
    }
}

void PartialView::PrintPartialView() {
    if (eclipseLogTime<=0) return;
    std::cout << "[ECLIPSE] "<< utility::GetCurrentTime() << " ";
    for (int i=0; i<active.size(); i++) {
        std::cout << active[i]->id << ",";
    }
    for (int i=0; i<passive.size(); i++) {
        std::cout << passive[i]->id << ",";
    }
    std::cout << "\n";
}

/* ------ Functions related to MessageStorage operation ----- */
std::string MessageStorage::GetSerializedMsg(Message* msg) {
    std::string serial_str;
    boost::iostreams::back_insert_device<std::string> inserter(serial_str);
    boost::iostreams::stream<boost::iostreams::back_insert_device<std::string> > s(inserter);
    boost::archive::binary_oarchive oa(s);
    oa << msg;
    s.flush();
    return serial_str;
}

bool MessageStorage::IsExist(size_t mid) {
    for (int i=storage.size()-1; i>=0; i--) {
        if (storage[i].mid == mid) return true;
    }
    return false;
}

IHAVE_MESSAGE* MessageStorage::GetMessageEntry(size_t mid) {
    for (int i=storage.size()-1; i>=0; i--) {
        if (storage[i].mid == mid) return &storage[i];
    }
    return NULL;
}

size_t MessageStorage::CreateNewId(Message* msg) {
    std::tr1::hash<std::string> str_hash;
    std::string smsg = GetSerializedMsg(msg);
    std::string ship = NodeInfo::GetInstance()->GetHostIP();
    return str_hash(smsg + ship);
}

void MessageStorage::AddToStorage(size_t mid, int round, std::string sender, Message* msg) {
    if (storage.size() == STORAGE_SIZE) {
        Message* msg = storage.begin()->msg;
        storage.erase(storage.begin());
        delete msg;
    }
    storage.emplace_back(mid, round, sender, msg);
}

/* ------ Functions related to RecoveryStorage operation ----- */
void RecoveryStorage::StartTimer(size_t mid) {
    auto entry_it = storage.find(mid);
    assert(entry_it != storage.end());

    RecoveryEntry* entry = entry_it->second[0];
    entry->timer_phase = 1;
    entry->recovery_timer.data = entry;

    ev_timer_init(&entry->recovery_timer, RecoveryTimeoutCallback, RECOVERY_TIMEOUT, 0);
    ev_timer_start(GlobalEvent::loop, &entry->recovery_timer);
}

void RecoveryStorage::RecoveryTimeoutCallback(EV_P_ ev_timer *w, int revents) {
    RecoveryEntry* entry = (RecoveryEntry*)w->data;
    if (entry->timer_phase == 1) {
        GossipModuleHeader newheader = GossipModuleHeader(CASTTYPE_UNICAST, PROTOCOL_GOSSIP);
        newheader.SetDstId(entry->sender);
        newheader.SetSrcId(NodeInfo::GetInstance()->GetHostIP());
        newheader.IncreaseHopCount();
        GossipModuleMessage newmsg = GossipModuleMessage(GOSSIP_GRAFT);
        newmsg.SetOption(entry->mid);

        HandleNetwork_GossipModel* instance = HandleNetwork_GossipModel::GetInstance();
        instance->UnicastWithHeader(&newheader, &newmsg);

        ev_timer_set(w, RECOVERY_TIMEOUT, 0);
        ev_timer_start(GlobalEvent::loop, w);
        entry->timer_phase = 2;
    }
    else if (entry->timer_phase == 2) {
        std::cout << "[RecoveryTimeoutCallback] Original Msg Not arrived\n";
    }
    else {
        std::cout << "[RecoveryTimeoutCallback] Invalid timer phase\n";
    }
}

RecoveryEntry* RecoveryStorage::FindFirstRecoveryEntry(size_t mid) {
    auto entry_it = storage.find(mid);
    if (entry_it == storage.end()) return NULL;
    else return entry_it->second[0];
}

void RecoveryStorage::AddToStorage(size_t mid, int round, std::string sender) {
    auto entry_it = storage.find(mid);
    if (entry_it == storage.end()) {
        storage[mid] = std::vector<RecoveryEntry*>();
        RecoveryEntry* entry = new RecoveryEntry(mid, round, sender);
        storage[mid].push_back(entry);
        StartTimer(mid);
    }
    else {
        RecoveryEntry* entry = new RecoveryEntry(mid, round, sender);
        storage[mid].push_back(entry);
    }
}

bool RecoveryStorage::StopTimerAndRemoveEntries(size_t mid) {
    auto entry_it = storage.find(mid);
    if (entry_it == storage.end()) return false;

    std::vector<RecoveryEntry*>& relist = entry_it->second;
    while(!relist.empty()){
        RecoveryEntry* temp = relist[0];
        if (temp->timer_phase != 0) {
            ev_timer_stop(GlobalEvent::loop, &temp->recovery_timer);
        }
        relist.erase(relist.begin());
        delete(temp);
    }
    assert(relist.size() == 0);
    storage.erase(mid);
    return true;
}
