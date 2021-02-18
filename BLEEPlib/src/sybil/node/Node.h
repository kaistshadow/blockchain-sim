//
// Created by ilios on 21. 2. 17..
//

#ifndef BLEEP_NODE_H
#define BLEEP_NODE_H

namespace libBLEEP_sybil {
    class Node {
    protected:
        std::string _myIP;
    public:
        Node(std::string vIP) : _myIP(vIP) {}

        virtual std::string GetIP() = 0;
    };
}

#endif //BLEEP_NODE_H
