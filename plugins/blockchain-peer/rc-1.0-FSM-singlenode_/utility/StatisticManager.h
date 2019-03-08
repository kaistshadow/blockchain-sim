#ifndef STATISTIC_MANAGER_H
#define STATISTIC_MANAGER_H

#include <map>
#include <string>
#include <vector>

class StatisticManager {
 protected:
    std::map<std::string, int> mapIntStats;

 public:
    
};

extern StatisticManager gStats;


#endif
