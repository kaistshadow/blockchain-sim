#include "Random.h"
#include "shadow_interface.h"

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

libBLEEP::random_source::random_source(int host_id) {
    struct timespec cur;
    clock_gettime(CLOCK_MONOTONIC, &cur);
    unsigned int randtime = (unsigned int)cur.tv_nsec;
    unsigned int random_num = randtime + host_id;
    static std::default_random_engine generator(random_num);
    default_random_source = &generator;
}
double libBLEEP::random_source::get_normal_value(double avg, double stddev, int flag) {
    double result = -1;
    bool continueFlag = 0;
    do {
        std::normal_distribution<double> distribution(avg, stddev);
        result = distribution(*this->default_random_source);
        continueFlag = (flag & RAND_DROP_NEGATIVE) && result < 0;
        if(continueFlag) {
            usleep(1);
            continue;
        }
    } while(continueFlag);
    return result;
}
double libBLEEP::random_source::get_exp_value(double lambda) {
    double result = -1;
    std::exponential_distribution<double> distribution(lambda);
    result = distribution(*this->default_random_source);
    return result;
}
libBLEEP::random_source & libBLEEP::get_global_random_source(int host_id) {
    static libBLEEP::random_source global_random_source(host_id);
    return global_random_source;
}
