#include <iostream>
#include <stdlib.h>
#include <unistd.h>

#include <errno.h> 
#include <string.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <sys/wait.h> 
#include <fcntl.h> /* Added for the nonblocking socket */
#include <arpa/inet.h>
#include <netdb.h>


#include <map>
#include <vector>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/device/back_inserter.hpp>


#include "blockchain/ledgermanager.h"
#include "blockchain/transaction.h"
#include "blockchain/block.h"
#include "consensus/stellarconsensusdriver.h"

#define MYPORT 3456    /* the port users will be connecting to */
#define BACKLOG 10     /* how many pending connections queue will hold */

using namespace std;

int client_sfd;
map< string, int > socket_m;

void DumpBlockchain(char *argv[]);

int main(int argc, char *argv[]) {
    // int nodeid = atoi(argv[1]);
    cout << "Dumping blockchain!\n";

    DumpBlockchain(argv);
}

void DumpBlockchain(char *argv[]) {

    {
        std::list<Transaction> tx_list;
        Transaction tx(1,2,100);
        Transaction tx2(3,4,200);
        tx_list.push_back(tx);
        tx_list.push_back(tx2);
        Block blk1("0", tx_list);
        
        std::list<Transaction> tx_list2;
        Transaction tx3(2,1,50);
        Transaction tx4(3,5,1000);
        tx_list2.push_back(tx3);
        tx_list2.push_back(tx4);
        Block blk2("1", tx_list2);

        std::list<Block> blk_list;
        blk_list.push_back(blk1);
        blk_list.push_back(blk2);

        StellarConsensusDriver driver;
        LedgerManager::SetInstance(driver, "blk.dat");
        LedgerManager::GetInstance()->SetLedger(blk_list);
        LedgerManager::GetInstance()->SaveLedgerToFile();
    }


    {

    }    

}
