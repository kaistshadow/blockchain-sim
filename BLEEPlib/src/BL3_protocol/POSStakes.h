//
// Created by Hyunjin Kim on 21. 6. 6..
//

// setup initial/dynamic stakes that each node knows
// support leader selection process related API

#ifndef BLEEP_POSSTAKES_H
#define BLEEP_POSSTAKES_H

#include <map>

namespace libBLEEP_BL {
    // assume that the access to the StakeList is not parallel.
	class StakeList {
	private:
	    bool fDirty;
	    std::map<unsigned int, unsigned int> leadermap;
	    std::map<unsigned int, unsigned int> inverted_leadermap;
	    unsigned int totalStakedValue;

	    void updateInvertedMap();
	public:
	    StakeList() {
	        fDirty = true;
	        totalStakedValue = 0;
	    }
	    void addStake(unsigned int leader, unsigned int stakedValue);
	    /* leader pick functions */
	    unsigned int first();
	    unsigned int last();
	    unsigned int pickLeader(unsigned int v);
	    unsigned int getTotal();
	};
}

#endif //BLEEP_POSSTAKES_H
