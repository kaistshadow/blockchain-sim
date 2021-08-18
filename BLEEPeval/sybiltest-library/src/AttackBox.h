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
        std::vector<ev::timer *> backupTimers;
        int backuptime = 0;
        AttackBox() : AttackPolicy<NodePrimitives, NodeParams>() {}

        void setTarget(std::string targetIP, int targetPort) {
            _targetIP = targetIP;
            _targetPort = targetPort;
        }
        void BackupPeersDat(ev::timer &w, int revents) {
            std::string datpath = "";
            // copy file https://stackoverflow.com/a/10195497
            std::ifstream src(datpath + "peers.dat", std::ios::binary);
            std::ofstream dst(datpath + "peers_backup_" + std::to_string(backuptime) + ".dat", std::ios::binary);
            dst << src.rdbuf();
            backuptime++;
            delete &w;
        }
        bool setupAttack() {
            if (_targetIP == "" || _targetPort == -1)
                return false;

            if (!AttackPolicy<NodePrimitives, NodeParams>::ConstructSybilNet(_ipDatabase, _targetIP,
                                                                             _targetPort))
                return false;
            for(int i=0; i < 1000; i++) {
                // a thousand days is enough
                ev::timer *backuptimer = new ev::timer();
                backuptimer->set<AttackBox, &AttackBox::BackupPeersDat>(this);
                backuptimer->set(24 * 60 * 60 * i, 0.);
                backuptimer->start();
                backupTimers.push_back(backuptimer);
            }
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
