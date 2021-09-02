// "Copyright [2021] <kaistshadow>"

//
// Created by ilios on 21. 2. 15..
//

#ifndef BLEEPEVAL_SYBILTEST_LIBRARY_SRC_ATTACKBOX_H_
#define BLEEPEVAL_SYBILTEST_LIBRARY_SRC_ATTACKBOX_H_

#include <string>
#include <memory>
#include <iostream>
#include "ipdb/SimpleIPDatabase.h"
#include <ev++.h>

namespace sybiltest {
template<template<class, class> class AttackPolicy,
        class NodePrimitives,
        class NodeParams,
        class IPDB>
class AttackBox : public AttackPolicy<NodePrimitives, NodeParams> {
 private:
    std::string _targetIP = "";
    int _targetPort = -1;
    IPDB _ipDatabase;

 public:
    AttackBox() : AttackPolicy<NodePrimitives, NodeParams>() {}

    void setTarget(std::string targetIP, int targetPort) {
        _targetIP = targetIP;
        _targetPort = targetPort;
    }

    bool setupAttack() {
        if (_targetIP == "" || _targetPort == -1)
            return false;

        if (!AttackPolicy<NodePrimitives, NodeParams>::ConstructSybilNet(_ipDatabase, _targetIP,
                                                                            _targetPort))
            return false;
        return true;
    }

    int startAttack() {
        ev_run(AttackPolicy<NodePrimitives, NodeParams>::libev_loop, 0);
        std::cout << "all watchers are removed or attack routine is finished" << "\n";
        return 0;
    }
};
}  // namespace sybiltest

#endif  // BLEEPEVAL_SYBILTEST_LIBRARY_SRC_ATTACKBOX_H_
