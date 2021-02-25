//
// Created by ilios on 21. 2. 15..
//

#ifndef BLEEP_ATTACKBOX_H
#define BLEEP_ATTACKBOX_H

#include <memory>
#include <iostream>
#include "ipdb/SimpleIPDatabase.h"
#include <ev++.h>

namespace libBLEEP_sybil {

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
}

#endif //BLEEP_ATTACKBOX_H
