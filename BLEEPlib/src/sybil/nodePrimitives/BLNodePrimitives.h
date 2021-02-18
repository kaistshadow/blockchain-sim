//
// Created by ilios on 21. 2. 15..
//

#ifndef BLEEP_ASYNC_CALLBACKS_H
#define BLEEP_ASYNC_CALLBACKS_H

#include <ev++.h>
#include <memory>
#include "../node/Node.h"

namespace libBLEEP_sybil {
    class BLNodePrimitives {
    private:
        Node *_node;

    public:
        BLNodePrimitives(Node *node) : _node(node) {}

        void OpAfterConnect();

        void CallbackAfterReceive();
    };
}

#endif //ASYNC_CALLBACKS_H
