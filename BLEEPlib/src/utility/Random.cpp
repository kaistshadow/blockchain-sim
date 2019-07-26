#include "Random.h"

std::list<int> libBLEEP::GenRandomNumSet(int maxNum, int maxCount){
    srand(time(NULL));
    std::set<int> numSet;
    std::list<int> numList;
    int maxSize = (maxNum < maxCount) ? maxNum : maxCount;
    if (maxNum == maxCount){
        for(int i = 0; i < maxSize; i++) numList.push_back(i);
    }else{
        while(1){
            int num = rand() % maxNum;
            if (numSet.insert(num).second) 
                numList.push_back(num);
            if(int(numSet.size()) == maxSize) break;
        }
    }
    return numList;
}
