//
// Created by csrc on 21. 3. 19..
//

#ifndef BLEEP_TXBOX_H
#define BLEEP_TXBOX_H

#include <iostream>
#include <TxBox.h>

namespace tpstest {


        template< template<class> class TPSPolicy, class NodePrimitives, class NodeTxgen >
        class TxBox : public TPSPolicy <NodePrimitives> {
            private:
            NodeTxgen* generator;
            std::string _targetIP = "";
            int _targetPort = -1;

            public:
            TxBox() {}

            bool bootstrap(const char* statefile, const char* keyfile) {
                generator = new NodeTxgen(statefile, keyfile);
            }
            // return data with hex string format
            std::string getNextTxStream() {
                return generator->generate();
            }

            void setTarget(std::string targetIP, int targetPort) {
              _targetIP = targetIP;
              _targetPort = targetPort;
            }

            bool setupAttack() {
              if (_targetIP == "" || _targetPort == -1)
                return false;

              printf("start ConstructNet\n");
              if (!TPSPolicy<NodePrimitives>::ConstructNet(_targetIP, _targetPort))
                return false;
              return true;
            }

            int startAttack() {
//              ev_run(TPSPolicy<NodePrimitives>::libev_loop, 0);
              std::cout << "all watchers are removed or attack routine is finished" << "\n";
              return 0;
            }
        };
}

#endif //BLEEP_TXBOX_H
