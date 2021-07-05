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
	    std::map<unsigned long, unsigned long> leadermap;
	    std::map<unsigned long, unsigned long> inverted_leadermap;
	    unsigned long totalStakedValue;

	    void updateInvertedMap();
	public:
	    StakeList() {
	        fDirty = true;
	        totalStakedValue = 0;
	    }
	    void addStake(unsigned long leader, unsigned long stakedValue);
	    /* leader pick functions */
	    unsigned long first();
	    unsigned long last();
	    unsigned long pickLeader(unsigned long v);
	    unsigned long getTotal();
	    void load(std::string stakefile);
		void show_stake_peerlist();
	};
}

#endif //BLEEP_POSSTAKES_H
