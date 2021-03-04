//
// Created by ilios on 21. 2. 19..
//

#ifndef BLEEP_SHADOWNODE_H
#define BLEEP_SHADOWNODE_H

#include "../utility/AttackStat.h"
#include "Node.h"
#include "shadow_interface.h"
#include <iostream>

namespace libBLEEP_sybil {
    template<class NodePrimitives>
    class ShadowNode : public Node<NodePrimitives> {

    public:
        ShadowNode(AttackStat *stat, IPDatabase *ipdb, std::string virtualIp, int listenPort = 0)
                : Node<NodePrimitives>(stat, ipdb, virtualIp, listenPort,
                                       NodeType::Shadow) {
        }

        // move constructor
        ShadowNode(ShadowNode &&other) = default;

    };
}

#endif //BLEEP_SHADOWNODE_H
