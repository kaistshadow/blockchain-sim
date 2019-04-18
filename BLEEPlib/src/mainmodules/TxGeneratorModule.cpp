#include "TxGeneratorModule.h"

using namespace libBLEEP;

void TxGeneratorModule::AsyncGenerateRandomTransaction(double time) {
    new AsyncGenerateRandomTransactionTimer(time, _mainEventManager);
}

