#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include "../utility/UInt256.h"

namespace libBLEEP_BL {
    /* class TransactionId { */
    /* public: */
    /*     TransactionId() {} */
    /*     virtual bool operator<(const TransactionId& id) const */
    /*     { */
            
    /*     }; */
    /* private: */
    /*     friend class boost::serialization::access; */
    /*     // When the class Archive corresponds to an output archive, the */
    /*     // & operator is defined similar to <<.  Likewise, when the class Archive */
    /*     // is a type of input archive the & operator is defined similar to >> */
    /*     template<class Archive> */
    /*         void serialize(Archive & ar, const unsigned int version) { */
    /*     } */
    /* }; */

    /* class Transaction { */
    /* private: */
    /*     TransactionId _id; */
    /* public: */
    /*     Transaction() {} */
    /*     Transaction(TransactionId id) { _id = id; } */

    /*     virtual TransactionId GetId() { return _id; } */
        
    /* private: */
    /*     friend class boost::serialization::access; */
    /*     // When the class Archive corresponds to an output archive, the */
    /*     // & operator is defined similar to <<.  Likewise, when the class Archive */
    /*     // is a type of input archive the & operator is defined similar to >> */
    /*     template<class Archive> */
    /*         void serialize(Archive & ar, const unsigned int version) { */
    /*         ar & _id; */
    /*     } */
    /* }; */

    class SimpleTransactionId {
    public:
        int sender;
        int receiver;
        float amount;

        SimpleTransactionId() {}
        SimpleTransactionId(int s, int r, float a) { sender = s; receiver = r; amount = a; }

        // This overloaded operator<< 
        // Defines a non-member function, and makes it a friend of this class at the same time. 
        friend std::ostream& operator<<(std::ostream &out, const SimpleTransactionId &tid) 
        {
            out << tid.sender << " sends " << tid.amount << " to " << tid.receiver;
            return out;
        }

        bool operator<(const SimpleTransactionId& id) const
        {
            if (sender != id.sender)
                return sender < id.sender;
            else if (receiver != id.receiver)
                return receiver < id.receiver;
            else
                return amount < id.amount;
        };
    private:
        friend class boost::serialization::access;
        // When the class Archive corresponds to an output archive, the
        // & operator is defined similar to <<.  Likewise, when the class Archive
        // is a type of input archive the & operator is defined similar to >>
        template<class Archive>
            void serialize(Archive & ar, const unsigned int version) {
            ar & sender;
            ar & receiver;
            ar & amount;
        }
    };

    class SimpleTransaction {
    private:
        SimpleTransactionId _id;
    public:
        SimpleTransaction() {}
        SimpleTransaction(int sid, int rid, float a) {
            sender_id=sid; receiver_id=rid; amount=a; 
            _id = SimpleTransactionId(sid, rid, a);
        }        
        int sender_id;
        int receiver_id;
        float amount;
        libBLEEP::UINT256_t tx_hash;

        SimpleTransactionId GetId() { return _id; }

        // This overloaded operator<< 
        // Defines a non-member function, and makes it a friend of this class at the same time. 
        friend std::ostream& operator<<(std::ostream &out, const SimpleTransaction &tx) 
        {
            out << tx.sender_id << " sends " << tx.amount << " to " << tx.receiver_id;
            return out;
        }

        void virtual SetTxHash(libBLEEP::UINT256_t hash) { tx_hash = hash; }
        virtual libBLEEP::UINT256_t GetTxHash() const { return tx_hash; }

    private:
        friend class boost::serialization::access;
        // When the class Archive corresponds to an output archive, the
        // & operator is defined similar to <<.  Likewise, when the class Archive
        // is a type of input archive the & operator is defined similar to >>
        template<class Archive>
            void serialize(Archive & ar, const unsigned int version) {
            ar & _id;
            ar & sender_id;
            ar & receiver_id;
            ar & amount;
            ar & tx_hash;
        }
    };

    
}

#endif
