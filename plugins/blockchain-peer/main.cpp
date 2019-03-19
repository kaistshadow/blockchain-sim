#include <iostream>
#include <stdlib.h>

#include "p2p/node.h"

using namespace std;

int main(int argc, char *argv[]) {
    int nodeid = atoi(argv[1]);
    cout << "Blockchain peer " << nodeid << " started!" << endl;

    NodeInit(nodeid);
    NodeLoop();
}
