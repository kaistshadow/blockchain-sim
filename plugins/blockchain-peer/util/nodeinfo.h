#ifndef NODEINFO_H
#define NODEINFO_H

#include <string>

class NodeInfo {
 private:
    NodeInfo() {}; // singleton pattern
    static NodeInfo* instance; // singleton pattern
    
    std::string myNodeId;

 public:
    static NodeInfo* GetInstance(); // sigleton pattern

    void SetHostId(std::string nodeid) { myNodeId = nodeid; }
    std::string GetHostId() { return myNodeId; }
};



#endif
