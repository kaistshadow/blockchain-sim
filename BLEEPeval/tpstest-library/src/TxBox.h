//
// Created by csrc on 21. 3. 19..
//

#ifndef BLEEP_TXBOX_H
#define BLEEP_TXBOX_H

#include <iostream>

namespace tpstest {

        template<class NodeTxgen, class NodeState, class NodeKey>
        class TxBox {
            private:
            NodeState* state;
            NodeKey* key;
            NodeTxgen generator;

            public:
            TxBox() {}

            bool bootstrap(const char* statefile, const char* keyfile) {
                state = new NodeState(statefile);
                key = new NodeKey(keyfile);
            }
            unsigned char* getNextTxStream(size_t& streamsize) {
                unsigned char* data = nullptr;
                NodeState* prevState = state;
                NodeKey* prevKey = key;
                data = generator.generate(&state, &key, streamsize);
                delete prevState;
                delete prevKey;
                return data;
            }
        };
}

#endif //BLEEP_TXBOX_H
