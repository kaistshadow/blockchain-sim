// "Copyright [2021] <kaistshadow>"

//
// Created by csrc on 21. 3. 19..
//

#ifndef BLEEPEVAL_PERFORMTEST_LIBRARY_SRC_TXBOX_H_
#define BLEEPEVAL_PERFORMTEST_LIBRARY_SRC_TXBOX_H_

#include <utility>
#include <string>
#include <iostream>
#include <vector>

namespace tpstest {
template< template<class> class TestPolicy, class NodePrimitives>
class TxBox : public TestPolicy <NodePrimitives> {
 private:
    std::string _txgenIP = "";
    std::string _monitorIP = "";
    int _nodePort = -1;
    std::vector<std::pair<std::string, int>> targets;

 public:
    TxBox() {}

    void addTarget(std::string targetIP, int targetPort) {
        std::cout << "addTarget : " << targetIP << " " << targetPort << "\n";
        targets.emplace_back(std::make_pair(targetIP, targetPort));
    }
    void addNode(std::string txgenIP, std::string mointorIP, int nodePort) {
        _txgenIP = txgenIP;
        _monitorIP = mointorIP;
        _nodePort = nodePort;
    }

    bool setupNetwork() {
        if (targets.size() == 0)
            return false;
        if (!TestPolicy<NodePrimitives>::ConstructNet(targets, _txgenIP, _monitorIP, _nodePort))
            return false;
        return true;
    }

    int startNetwork(bool txgenstart) {
        if (txgenstart) {
            TestPolicy<NodePrimitives>::SetGenerateTimer();
            TestPolicy<NodePrimitives>::setMonitorTimer();
        }
        ev_run(TestPolicy<NodePrimitives>::libev_loop, 0);
        return 0;
    }
};
} // namespace tpstest

#endif // BLEEPEVAL_PERFORMTEST_LIBRARY_SRC_TXBOX_H_
