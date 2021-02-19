//
// Created by ilios on 21. 2. 16..
//

#include <algorithm>
#include "Reactor.h"
#include <iostream>

#include <arpa/inet.h>
#include <unistd.h>


using namespace libBLEEP_sybil;
using namespace std;

struct ev_loop *Reactor::libev_loop = nullptr;
Reactor *Reactor::_instance = 0;

Reactor *Reactor::Instance() {
    if (_instance == 0) {
        _instance = new Reactor();
    }
    return _instance;
}

void Reactor::HandleEvents() {
    // ask the operating system for any new events,
    // call the watcher callbacks, and then repeat the whole process indefinitely.
    // it will keep handling events until either no event watchers are active anymore or ev_break was called.
    ev_run(libev_loop, 0);
}

bool Reactor::RegisterIOWatcher(ev::io *iowatcher) {
    auto it = find_if(_vIOWatcher.begin(), _vIOWatcher.end(),
                      [=](const ev::io *x) { return x == iowatcher; });
    if (it == _vIOWatcher.end()) {
        _vIOWatcher.push_back(iowatcher);
        return true;
    }
    return false;
}

bool Reactor::RemoveIOWatcher(ev::io *iowatcher) {
    auto it = find_if(_vIOWatcher.begin(), _vIOWatcher.end(),
                      [=](const ev::io *x) { return x == iowatcher; });
    if (it != _vIOWatcher.end()) {
        auto iow = *it;
        iow->stop();
        _vIOWatcher.erase(it);
        return true;
    }
    return false;
}

bool Reactor::RegisterTimerWatcher(ev::timer *timerwatcher) {
    auto it = find_if(_vTimerWatcher.begin(), _vTimerWatcher.end(),
                      [=](const ev::timer *x) { return x == timerwatcher; });
    if (it == _vTimerWatcher.end()) {
        _vTimerWatcher.push_back(timerwatcher);
        return true;
    }
    return false;
}

bool Reactor::RemoveTimerWatcher(ev::timer *timerwatcher) {
    auto it = find_if(_vTimerWatcher.begin(), _vTimerWatcher.end(),
                      [=](const ev::timer *x) { return x == timerwatcher; });
    if (it != _vTimerWatcher.end()) {
        auto iow = *it;
        iow->stop();
        _vTimerWatcher.erase(it);
        return true;
    }
    return false;
}

