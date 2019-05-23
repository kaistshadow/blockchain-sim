#include "Random.h"

std::set<int> libBLEEP::GenRandomNumSet(int maxNum, int maxCount){
    srand(time(NULL));
    std::set<int> numSet;
    int maxSize = (maxNum < maxCount) ? maxNum : maxCount;
    if (maxNum == maxCount){
        for(int i = 0; i < maxSize; i++) numSet.insert(i);
    }else{
        while(1){
            int num = rand() % maxNum;
            numSet.insert(num);
            if(int(numSet.size()) == maxSize) break;
        }
    }
    return numSet;
}
