#include "PBFTMsgHolder.h"

#include "PBFTSigning.h"

namespace libBLEEP_BL {
    int PBFTMsgHolder::addPreprepared(unsigned long v, unsigned int n, std::shared_ptr<PBFTBlock> m) {
        auto it = _preprepared.find({ v, n });
        if (it == _preprepared.end()) {
            _preprepared.insert({ {v, n}, m });
            return 1;
        }
        return 0;
    }
    int PBFTMsgHolder::addPrepared(unsigned long v, unsigned int n, std::string d, unsigned long i) {
        auto it = _prepared.find({ v, n });
        if (it == _prepared.end()) {
            std::shared_ptr<std::set<unsigned long>> s;
            std::shared_ptr<std::map<std::string, std::shared_ptr<std::set<unsigned long>>>> pm;
            s->insert(i);
            pm->insert({ d, s });
            _prepared.insert({ {v, n}, pm });
            return 1;
        }
        else {
            auto it2 = it->second->find(d);
            if (it2 == it->second->end()) {
                std::shared_ptr<std::set<unsigned long>> s;
                s->insert(i);
                it->second->insert({ d, s });
                return 1;
            }
            else {
                auto it3 = it2->second->find(i);
                if (it3 == it2->second->end()) {
                    it2->second->insert(i);
                    return 1;
                }
                else {
                    return 0;
                }
            }
        }
    }
    int PBFTMsgHolder::addCommit(unsigned long v, unsigned int n, std::string d, unsigned long i) {
        auto it = _committed.find({ v, n });
        if (it == _committed.end()) {
            std::shared_ptr<std::set<unsigned long>> s;
            std::shared_ptr<std::map<std::string, std::shared_ptr<std::set<unsigned long>>>> pm;
            s->insert(i);
            pm->insert({ d, s });
            _committed.insert({ {v, n}, pm });
            return 1;
        }
        else {
            auto it2 = it->second->find(d);
            if (it2 == it->second->end()) {
                std::shared_ptr<std::set<unsigned long>> s;
                s->insert(i);
                it->second->insert({ d, s });
                return 1;
            }
            else {
                auto it3 = it2->second->find(i);
                if (it3 == it2->second->end()) {
                    it2->second->insert(i);
                    return 1;
                }
                else {
                    return 0;
                }
            }
        }
    }

    bool PBFTMsgHolder::hasPreprepared(unsigned long v, unsigned int n) {
        auto it = _preprepared.find({ v, n });
        if (it != _preprepared.end()) {
            return true;
        }
        else {
            return false;
        }
    }

    // know m, preprepare for m with (v, n), 2 x f prepare(v, n, d, ...) from others
    bool PBFTMsgHolder::predPrepared(unsigned long v, unsigned int n) {
        auto ppr_it = _preprepared.find({ v, n });
        if (ppr_it == _preprepared.end()) {
            // does not know m
            return false;
        }
        auto m = ppr_it->second;
        std::string d = PBFTDigest(m->GetBlockHash().str());

        auto pr_it = _prepared.find({ v, n });
        if (pr_it == _prepared.end()) {
            return false;
        }
        auto prd_it = pr_it->second->find(d);
        if (prd_it == pr_it->second->end()) {
            return false;
        }
        auto prds = prd_it->second;
        if (prds->size() < 2 * _f) {
            return false;
        }

        return true;
    }
    // predPrepared(m, v, n, i), 2 x f + 1 commit(v, n, d) from others
    bool PBFTMsgHolder::predCommittedLocal(unsigned long v, unsigned int n) {
        auto ppr_it = _preprepared.find({ v, n });
        if (ppr_it == _preprepared.end()) {
            // does not know m
            return false;
        }
        auto m = ppr_it->second;
        std::string d = PBFTDigest(m->GetBlockHash().str());

        auto pr_it = _prepared.find({ v, n });
        if (pr_it == _prepared.end()) {
            return false;
        }
        auto prd_it = pr_it->second->find(d);
        if (prd_it == pr_it->second->end()) {
            return false;
        }
        auto prds = prd_it->second;
        if (prds->size() < 2 * _f) {
            return false;
        }

        auto cm_it = _committed.find({ v, n });
        if (cm_it == _committed.end()) {
            return false;
        }
        auto cmd_it = cm_it->second->find(d);
        if (cmd_it == cm_it->second->end()) {
            return false;
        }
        auto cmds = cmd_it->second;
        if (cmds->size() < 2 * _f + 1) {
            return false;
        }
        return true;
    }
}
