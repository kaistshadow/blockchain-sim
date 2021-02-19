//
// Created by ilios on 21. 2. 16..
//

#ifndef BLEEP_REACTOR_H
#define BLEEP_REACTOR_H

#include <ev++.h>
#include <memory>
#include <vector>


namespace libBLEEP_sybil {
    class Reactor {
    public:
        static Reactor *Instance();

        void HandleEvents();

        bool RegisterIOWatcher(ev::io *iowatcher);

        bool RemoveIOWatcher(ev::io *iowatcher);

        bool RegisterTimerWatcher(ev::timer *timerwatcher);

        bool RemoveTimerWatcher(ev::timer *timerwatcher);

    private:
        static Reactor *_instance;

        Reactor() { libev_loop = EV_DEFAULT; };

        std::vector<ev::io *> _vIOWatcher;
        std::vector<ev::timer *> _vTimerWatcher;
        static struct ev_loop *libev_loop;
    };


}

#endif //BLEEP_REACTOR_H
