//
// Created by ilios on 21. 2. 15..
//

#ifndef BLEEP_ATTACKBOX_H
#define BLEEP_ATTACKBOX_H

#include <memory>
#include <iostream>
#include "ipdb/SimpleIPDatabase.h"
#include <ev++.h>

// for backup peers.dat
#include <fstream>
#include <string>
#include <vector>
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
        ev::timer *backupTimer;
        int backuptime = 0;
        AttackBox() : AttackPolicy<NodePrimitives, NodeParams>() {}

        void setTarget(std::string targetIP, int targetPort) {
            _targetIP = targetIP;
            _targetPort = targetPort;
        }
        void BackupPeersDat(ev::timer &w, int revents) {
            std::string datpath = "data/bcdnode0/";
            // copy file https://stackoverflow.com/a/10195497
            std::ifstream src(datpath + "peers.dat", std::ios::binary);
            std::ofstream dst(datpath + "peers_backup_" + std::to_string(backuptime) + ".dat", std::ios::binary);
            dst << src.rdbuf();
            backuptime++;
        }
        bool setupAttack() {
            if (_targetIP == "" || _targetPort == -1)
                return false;

            if (!AttackPolicy<NodePrimitives, NodeParams>::ConstructSybilNet(_ipDatabase, _targetIP,
                                                                             _targetPort))
                return false;
            backupTimer = new ev::timer();
            backupTimer->set<AttackBox, &AttackBox::BackupPeersDat>(this);
            backupTimer->set(0, 24 * 60 * 60);
            backupTimer->start();
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
