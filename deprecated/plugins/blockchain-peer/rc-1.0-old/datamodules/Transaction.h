#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "../p2pnetwork/Message.h"

enum TransactionType {
    EnumSimpleTransaction,
    EnumUselessTransaction,
};

// Interface Class
class Transaction: public Message {
 public:
    Transaction() {}
    virtual int GetType() {return TX_MESSAGE;}
    virtual ~Transaction() {}

    // This overloaded operator<< 
    // Defines a non-member function, and makes it a friend of this class at the same time. 
    friend std::ostream& operator<<(std::ostream &out, const Transaction &t) 
    {
        return t.print(out);
    }
    // We'll rely on member function print() to do the actual printing
    // Because print is a normal member function, it can be virtualized
    virtual std::ostream& print(std::ostream&out) const = 0;

    
    // This overloads opeartor==
    // Defines a non-member function, and makes it a friend of this class at the same time. 
    friend bool operator==(Transaction const & lhs, Transaction const & rhs)
    {
        // make sure the types are the same
        if (typeid(lhs) != typeid(rhs))
            return false;

        // if they are the same, then compare them
        return lhs.compare(rhs);
    }

 private:
    virtual bool compare(Transaction const & rhs) const = 0;
                         

    friend class boost::serialization::access;
    // When the class Archive corresponds to an output archive, the
    // & operator is defined similar to <<.  Likewise, when the class Archive
    // is a type of input archive the & operator is defined similar to >>
    template<class Archive>
        void serialize(Archive & ar, const unsigned int version) {
        ar & boost::serialization::base_object<Message>(*this);
        boost::serialization::void_cast_register<Message,Transaction>();
    }
};

class SimpleTransaction: public Transaction {
 public:
    SimpleTransaction() {}
    SimpleTransaction(int sid, int rid, float a) {sender_id=sid; receiver_id=rid; amount=a; }

    virtual std::ostream& print(std::ostream& out) const override 
    {
        out << sender_id << " sends " << amount << " to " << receiver_id;
        return out;
    }
    int sender_id;
    int receiver_id;
    float amount;
 private:
    virtual bool compare(Transaction const & rhs) const 
    {
        // safe to staic cast because Transaction's == operator already confiremd the type
        const SimpleTransaction& tx = static_cast<const SimpleTransaction&>(rhs);
        if (sender_id == tx.sender_id &&
            receiver_id == tx.receiver_id &&
            amount == tx.amount)
            return true;
        else
            return false;
    }

    friend class boost::serialization::access;
    // When the class Archive corresponds to an output archive, the
    // & operator is defined similar to <<.  Likewise, when the class Archive
    // is a type of input archive the & operator is defined similar to >>
    template<class Archive>
        void serialize(Archive & ar, const unsigned int version) {
        ar & boost::serialization::base_object<Transaction>(*this);
        boost::serialization::void_cast_register<Transaction,SimpleTransaction>();
        ar & sender_id;
        ar & receiver_id;
        ar & amount;
    }
};

class UselessTransaction: public Transaction {
 public:
    UselessTransaction() {}
    UselessTransaction(int id) {random_id=id; }

    virtual std::ostream& print(std::ostream& out) const override 
    {
        out << "random_id:" << random_id;
        return out;
    }
    int random_id;
 private:
    virtual bool compare(Transaction const & rhs) const 
    {
        // safe to staic cast because Transaction's == operator already confiremd the type
        const UselessTransaction& tx = static_cast<const UselessTransaction&>(rhs);
        if (random_id == tx.random_id)
            return true;
        else
            return false;
    }

    friend class boost::serialization::access;
    // When the class Archive corresponds to an output archive, the
    // & operator is defined similar to <<.  Likewise, when the class Archive
    // is a type of input archive the & operator is defined similar to >>
    template<class Archive>
        void serialize(Archive & ar, const unsigned int version) {
        ar & boost::serialization::base_object<Transaction>(*this);
        boost::serialization::void_cast_register<Transaction,UselessTransaction>();
        ar & random_id;
    }
};

/* std::ostream& operator<<(std::ostream&, const Transaction&); // in order to overload the << operator */

/* bool operator<(Transaction const & lhs, Transaction const & rhs); */
/* bool operator==(Transaction const & lhs, Transaction const & rhs); */

#endif
