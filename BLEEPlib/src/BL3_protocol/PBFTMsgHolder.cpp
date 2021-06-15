#include "PBFTMsgHolder.h"

namespace libBLEEP_BL {
	bool PBFTMsgHolder::hasPreprepared(unsigned int n) {
		auto it = _preprepared.find(n);
		return it != _preprepared.end();
	}
	void PBFTMsgHolder::addPreprepared(unsigned int n, std::string d) {
		_preprepared.insert({n, d});
	}
	void PBFTMsgHolder::clearPreprepared() {
		_preprepared.clear();
	}
}