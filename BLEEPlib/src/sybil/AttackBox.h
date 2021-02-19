//
// Created by ilios on 21. 2. 15..
//

#ifndef BLEEP_ATTACKBOX_H
#define BLEEP_ATTACKBOX_H

#include <memory>
#include <iostream>
#include "ipdb/IPDatabase.h"
#include <ev++.h>

namespace libBLEEP_sybil {

    template<template<class> class AttackPolicy,
            class NodePrimitives,
            class IPDB>
    class AttackBox : public AttackPolicy<NodePrimitives> {
    private:
        std::string _targetIP = "";
        int _targetPort = -1;
        IPDB _ipDatabase;

        struct ev_loop *libev_loop = nullptr;

    public:
        AttackBox() : AttackPolicy<NodePrimitives>() { libev_loop = EV_DEFAULT; }

        void setTarget(std::string targetIP, int targetPort) {
            _targetIP = targetIP;
            _targetPort = targetPort;
        }

        std::string getTargetIP() { return _targetIP; }

        int getTargetPort() { return _targetPort; }

        bool setupAttack() {
            if (_targetIP == "" || _targetPort == -1)
                return false;
            if (!AttackPolicy<NodePrimitives>::ConstructSybilNet(_ipDatabase.GetVReachableIP(), _targetIP, _targetPort))
                return false;
            return true;
        }

        int startAttack() {
            ev_run(libev_loop, 0);
            std::cout << "all watchers are removed" << "\n";
            return 0;
        }
    };
}

#endif //BLEEP_ATTACKBOX_H
