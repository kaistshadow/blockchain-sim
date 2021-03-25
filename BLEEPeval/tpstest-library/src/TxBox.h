//
// Created by csrc on 21. 3. 19..
//

#ifndef BLEEP_TXBOX_H
#define BLEEP_TXBOX_H

#include <iostream>
#include <TxBox.h>
#include <vector>

namespace tpstest {


        template< template<class> class TPSPolicy, class NodePrimitives, class NodeTxgen >
        class TxBox : public TPSPolicy <NodePrimitives> {
            private:
            NodeTxgen* generator;
            std::string _nodeIP = "";
            int _nodePort = -1;
            std::vector<std::pair<std::string, int>> targets;

            public:
            TxBox() {}

            bool bootstrap(const char* statefile, const char* keyfile) {
                generator = new NodeTxgen(statefile, keyfile);
            }
            // return data with hex string format
            std::string getNextTxStream() {
                return generator->generate();
            }

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

                if (!TPSPolicy<NodePrimitives>::ConstructNet(targets, _nodeIP, _nodePort))
                  return false;
              return true;
            }

            int startNetwork() {
              ev_run(TPSPolicy<NodePrimitives>::libev_loop, 0);
              std::cout << "all watchers are removed or attack routine is finished" << "\n";
              return 0;
            }
        };
}

#endif //BLEEP_TXBOX_H
