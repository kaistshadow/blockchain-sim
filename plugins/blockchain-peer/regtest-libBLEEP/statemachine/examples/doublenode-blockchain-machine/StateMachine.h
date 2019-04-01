#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <map>
#include <boost/signals2/signal.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <iostream>


#include "State.h"
#include "StateHandler.h"


#include "datamanagermodules/ShadowPipeManager.h"
#include "datamanagermodules/PipeManager.h"
#include "datamanagermodules/TxPool.h"
#include "datamanagermodules/LedgerManager.h"
#include "datamanagermodules/SocketManager.h"
#include "datamanagermodules/PeerManager.h"

#include "datamodules/Block.h"
#include "datamodules/Transaction.h"


namespace doublenode_blockchain_machine {
    /* define your customized data, datamanagers.
       For example, transaction, block, ledgermanager, serialization, deserialization, etc */

    class MyBlock : public libBLEEP::Block {
    private:
        unsigned long block_idx;
    public:
    MyBlock() : libBLEEP::Block() { }
    MyBlock(std::string id, std::list<boost::shared_ptr<libBLEEP::Transaction> > input_tx_list) : libBLEEP::Block(id, input_tx_list) { }
        unsigned long GetBlockIdx() const {return block_idx; }
        void SetBlockIdx(unsigned long idx) { block_idx = idx; }

    private:
        friend class boost::serialization::access;
        // When the class Archive corresponds to an output archive, the
        // & operator is defined similar to <<.  Likewise, when the class Archive
        // is a type of input archive the & operator is defined similar to >>
        template<class Archive>
            void serialize(Archive & ar, const unsigned int version) {
            ar & boost::serialization::base_object<Block>(*this);
            boost::serialization::void_cast_register<Block,MyBlock>();
            ar & block_idx;
        }
    };

    class MyLedgerManager : public libBLEEP::ListLedgerManager<MyBlock> {
    private:
    public:
    MyLedgerManager() : libBLEEP::ListLedgerManager<MyBlock>() {}
        void ReplaceLastBlock(std::shared_ptr<MyBlock> validBlock) {
            list_ledger.pop_back();
            list_ledger.push_back(*validBlock);
        }
    };

    std::string GetSerializedString(std::shared_ptr<MyBlock> blk);
    std::shared_ptr<MyBlock> GetDeserializedMyBlock(std::string str);


    typedef boost::signals2::signal<StateEnum () > StateSignal;

    class StateMachine {
    public:
        // for shadow pipe structures
        libBLEEP::ShadowPipeManager shadowPipeManager;

        // for normal pipe structures
        libBLEEP::PipeManager pipeManager;
        // for pipe test
        libBLEEP::PipeID testPipeID;

        // for pending-transaction management
        libBLEEP::TxPool txPool;

        // for ledger management
        /* libBLEEP::LedgerManager ledgerManager; */
        MyLedgerManager ledgerManager;

        // for listening socket management
        libBLEEP::ListenSocketManager listenSocketManager;

        // for connecting socket management
        libBLEEP::ConnectSocketManager connectSocketManager;

        // for data socket management
        libBLEEP::DataSocketManager dataSocketManager;

        // for peer management
        libBLEEP::PeerManager peerManager;

    public:
        // state variables
        bool leaderMachine = false; // indicate the leader machine among double node consensus

    public:
        // pointer for intermediate values (arguments passing between states)
        std::shared_ptr<MyBlock> newBlock = nullptr;
        std::shared_ptr<MyBlock> receivedBlock = nullptr;

    private:
        // for logic 
        StateEnum curState;
        StateEnum nextState;

    public:
        StateMachine();
        void InitStateMachine();
        void StartStateMachine();

        std::map< StateEnum, StateSignal > stateToSignalMap;   // For each state, there exists a callback function.

    };


    extern StateMachine gStateMachine;

}

#endif
