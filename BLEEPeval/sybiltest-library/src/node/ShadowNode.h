// "Copyright [2021] <kaistshadow>"

//
// Created by ilios on 21. 2. 19..
//

#ifndef BLEEPEVAL_SYBILTEST_LIBRARY_SRC_NODE_SHADOWNODE_H_
#define BLEEPEVAL_SYBILTEST_LIBRARY_SRC_NODE_SHADOWNODE_H_

#include <string>
#include "../utility/AttackStat.h"
#include "Node.h"
#include "shadow_interface.h"
#include <iostream>

namespace sybiltest {
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
}  // namespace sybiltest

#endif  // BLEEPEVAL_SYBILTEST_LIBRARY_SRC_NODE_SHADOWNODE_H_
