#include "HandleTransaction.h"
#include "HandleTransaction_BasicModel.h"

std::shared_ptr<HandleTransaction>  HandleTransaction::create(const int type) {
    if (type == HANDLE_TRANSACTION_BASICMODEL) {
        return std::shared_ptr<HandleTransaction>(new HandleTransaction_BasicModel());
    }
    else {
        exit(-1);
    }
}
