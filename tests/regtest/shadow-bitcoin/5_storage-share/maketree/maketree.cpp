#include <iostream>
#include <map>
#include <string>
#include <string.h>

class Node{
public:
    int height;
    int txCnt;
    int totalTxCnt;
    char* prevblockhash;
    char* blockhash;
};

std::map<std::string, Node> block_map;
int getPrevBlockTxcount(char* prevblockhash) {
    if(memcmp(prevblockhash,"0000000000000000000000000000000000000000000000000000000000000000",sizeof(char)*32)==0) {
        return 0;
    }
    int prevblocktxcount = block_map.find(prevblockhash)->second.totalTxCnt;
    return prevblocktxcount;

}

Node newblock(char* prevblockhash, char* blockhash, int txcount, int height){
    Node node;
    node.height = height;
    node.txCnt = txcount;
    node.totalTxCnt = getPrevBlockTxcount(prevblockhash)+txcount;
    node.prevblockhash=(char*)malloc(sizeof(char)*32);
    node.prevblockhash=prevblockhash;
    node.blockhash=(char*)malloc(sizeof(char)*32);
    node.blockhash=blockhash;
    return node;
}

int insertblock(char* prevblockhash, char* blockhash, int txcount,int height) {
    std::map<std::string, Node>::iterator it;
    Node node;
    node= newblock(prevblockhash,blockhash,txcount,height);
    block_map[blockhash]=node;
    it=block_map.find(prevblockhash);
    std::cout<<"find : "<<prevblockhash<< " -> "<<it->second.totalTxCnt<<std::endl;
}

void parsing_txcount() {
//    char input[100];
    std::string nodename="0";
    std::string input;
    input= "stdout-bcdnode0.bitcoind.0.log";
    input="../datadir/hosts/bcdnode"+nodename+"/stdout-bcdnode"+nodename+".bitcoind."+nodename+".log";

    FILE *inputFile = fopen(input.c_str(),"r");
    if (inputFile){
        while(!feof(inputFile)) {
            char buf[256];
            fgets(buf, sizeof(buf), inputFile);
            char *ptr = strtok(buf, " ");
            if (ptr) {
                ptr = strtok(NULL, " ");
                if (ptr) {
                    if (memcmp(ptr, "WriteBlockToDisk-txCount", sizeof("WriteBlockToDisk-txCount")) == 0) {
                        char *prevblockhash = strtok(NULL, " ");
                        char *blockhash = strtok(NULL, " ");
                        int txcnt = atoi(strtok(NULL, " "));
                        int height = atoi(strtok(NULL, " "));
                        insertblock(prevblockhash, blockhash, txcnt, height);
                    } else continue;
                }
            }
        }
        fclose(inputFile);
    }
}

int main(){
    parsing_txcount();
}




