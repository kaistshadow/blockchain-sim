//
// Created by csrc on 21. 3. 19..
//

#ifndef BLEEP_TXBOX_H
#define BLEEP_TXBOX_H

#include <iostream>

namespace tpstest {

        template<class NodeTxgen>
        class TxBox {
            private:
            NodeTxgen* generator;

            public:
            TxBox() {}

            bool bootstrap(const char* statefile, const char* keyfile) {
                generator = new NodeTxgen(statefile, keyfile);
            }
            // return data with hex string format
            std::string getNextTxStream() {
                return generator->generate();
            }
        };
}

#endif //BLEEP_TXBOX_H
