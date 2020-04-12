#ifndef HANDLE_TRANSACTION_H
#define HANDLE_TRANSACTION_H

#include <memory>
#include <boost/shared_ptr.hpp>

#include "../datamodules/Transaction.h"

#define HANDLE_TRANSACTION_BASICMODEL 0

class HandleTransaction {
 public:
    virtual ~HandleTransaction() {};
    static std::shared_ptr<HandleTransaction> create(const int type);

    virtual void HandleArrivedTx(boost::shared_ptr<Transaction> tx) = 0;
    virtual boost::shared_ptr<Transaction> MakeRandomValidTransaction() = 0;

};


#endif // HANDLE_NETWORK_H
