#include<iostream>
#include<set>
#include <list>
#include <random>

namespace libBLEEP {

    std::list<int> GenRandomNumSet(int maxNum, int maxCount);

#define RAND_DROP_NO        0
#define RAND_DROP_NEGATIVE  1
    class random_source {
    private:
        std::default_random_engine* default_random_source = NULL;
    public:
        random_source();
        double get_normal_value(double avg, double stddev, int flag);
        double get_exp_value(double lambda);
    };
    random_source& get_global_random_source();
}


