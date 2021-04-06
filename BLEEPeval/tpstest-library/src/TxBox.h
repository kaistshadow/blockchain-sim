//
// Created by csrc on 21. 3. 19..
//

#ifndef BLEEP_TXBOX_H
#define BLEEP_TXBOX_H

#include <iostream>
#include <vector>

namespace tpstest {
        template< template<class> class TestPolicy, class NodePrimitives>
        class TxBox : public TestPolicy <NodePrimitives> {
            private:
            std::string _nodeIP = "";
            int _nodePort = -1;
            std::vector<std::pair<std::string, int>> targets;

            public:
            TxBox() {}

            void addTarget(std::string targetIP, int targetPort) {
                std::cout<<"addTarget : "<<targetIP <<" "<< targetPort<<"\n";
                targets.emplace_back(std::make_pair(targetIP, targetPort));
            }
            void addNode(std::string nodeIP, int nodePort) {
                _nodeIP = nodeIP;
                _nodePort = nodePort;
            }

            bool setupNetwork() {
                if (targets.size()==0)
                    return false;

                if (!TestPolicy<NodePrimitives>::ConstructNet(targets, _nodeIP, _nodePort))
                    return false;
                return true;
            }

            int startNetwork() {
                ev_run(TestPolicy<NodePrimitives>::libev_loop, 0);
                return 0;
            }
        };
}

#endif //BLEEP_TXBOX_H
