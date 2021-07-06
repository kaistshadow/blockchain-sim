#include "PBFTMsgHolder.h"

#include "PBFTSigning.h"

#include <iostream>

namespace libBLEEP_BL {
    int PBFTViewMsgHolder::addPreprepared(unsigned int n, std::shared_ptr<PBFTBlock> m) {
        std::cout << "Debug PBFTViewMsgHolder::addPreprepared called\n";
        auto it = _preprepared.find(n);
        if (it == _preprepared.end()) {
            std::cout << "Debug PBFTViewMsgHolder::addPreprepared::no_preprepared exists\n";
            _preprepared.insert({ n, m });
            return 1;
        }
        return 0;
    }
    int PBFTViewMsgHolder::addPrepared(unsigned int n, std::string d, unsigned long i) {
        std::cout << "Debug PBFTViewMsgHolder::addPrepared called\n";
        auto it = _prepared.find(n);
        if (it == _prepared.end()) {
            std::cout << "Debug PBFTViewMsgHolder::addPrepared::no it\n";
            std::shared_ptr<std::set<unsigned long>> s = std::make_shared<std::set<unsigned long>>();
            std::shared_ptr<std::map<std::string, std::shared_ptr<std::set<unsigned long>>>> pm = std::make_shared<std::map<std::string, std::shared_ptr<std::set<unsigned long>>>>();
            s->insert(i);
            pm->insert({ d, s });
            _prepared.insert({ n, pm });
            return 1;
        }
        else {
            auto it2 = it->second->find(d);
            if (it2 == it->second->end()) {
                std::cout << "Debug PBFTViewMsgHolder::addPrepared::no it2\n";
                std::shared_ptr<std::set<unsigned long>> s = std::make_shared<std::set<unsigned long>>();
                s->insert(i);
                it->second->insert({ d, s });
                return 1;
            }
            else {
                auto it3 = it2->second->find(i);
                if (it3 == it2->second->end()) {
                    std::cout << "Debug PBFTViewMsgHolder::addPrepared::no it3\n";
                    it2->second->insert(i);
                    return 1;
                }
                else {
                    std::cout << "Debug PBFTViewMsgHolder::addPrepared::element exists\n";
                    return 0;
                }
            }
        }
    }
    int PBFTViewMsgHolder::addCommit(unsigned int n, std::string d, unsigned long i) {
        std::cout << "Debug PBFTViewMsgHolder::addCommit called\n";
        auto it = _committed.find(n);
        if (it == _committed.end()) {
            std::cout << "Debug PBFTViewMsgHolder::addCommit::no it\n";
            std::shared_ptr<std::set<unsigned long>> s = std::make_shared<std::set<unsigned long>>();
            std::shared_ptr<std::map<std::string, std::shared_ptr<std::set<unsigned long>>>> pm = std::make_shared<std::map<std::string, std::shared_ptr<std::set<unsigned long>>>>();
            s->insert(i);
            pm->insert({ d, s });
            _committed.insert({ n, pm });
            return 1;
        }
        else {
            auto it2 = it->second->find(d);
            if (it2 == it->second->end()) {
                std::cout << "Debug PBFTViewMsgHolder::addCommit::no it2\n";
                std::shared_ptr<std::set<unsigned long>> s = std::make_shared<std::set<unsigned long>>();
                s->insert(i);
                it->second->insert({ d, s });
                return 1;
            }
            else {
                auto it3 = it2->second->find(i);
                if (it3 == it2->second->end()) {
                    std::cout << "Debug PBFTViewMsgHolder::addCommit::no it3\n";
                    it2->second->insert(i);
                    return 1;
                }
                else {
                    std::cout << "Debug PBFTViewMsgHolder::addCommit::element exists\n";
                    return 0;
                }
            }
        }
    }

    bool PBFTViewMsgHolder::hasPreprepared(unsigned int n) {
        auto it = _preprepared.find(n);
        if (it != _preprepared.end()) {
            return true;
        }
        else {
            return false;
        }
    }
    bool PBFTViewMsgHolder::hasCommit(unsigned int n, std::string d, unsigned long i) {
        auto it = _committed.find(n);
        if (it == _committed.end()) {
            return false;
        }
        auto it2 = it->second->find(d);
        if (it2 == it->second->end()) {
            return false;
        }
        auto it3 = it2->second->find(i);
        if (it3 == it2->second->end()) {
            return false;
        }
        return true;
    }

    // know m, preprepare for m with (v, n), 2 x f prepare(v, n, d, ...) from others
    bool PBFTViewMsgHolder::predPrepared(unsigned int n) {
        std::cout << "predPrepared\n";
        auto ppr_it = _preprepared.find(n);
        if (ppr_it == _preprepared.end()) {
            // does not know m
            std::cout << "predPrepared::does not know m\n";
            return false;
        }
        auto m = ppr_it->second;
        std::string d = PBFTDigest(m->GetBlockHash().str());

        auto pr_it = _prepared.find(n);
        if (pr_it == _prepared.end()) {
            std::cout << "predPrepared::pr_it not found\n";
            return false;
        }
        auto prd_it = pr_it->second->find(d);
        if (prd_it == pr_it->second->end()) {
            std::cout << "predPrepared::prd_it not found\n";
            return false;
        }
        auto prds = prd_it->second;
        if (prds->size() < 2 * _f) {
            std::cout << "predPrepared::prepare count does not match to _f:" << (2*_f) << "\n";
            return false;
        }

        std::cout << "Result_prePrepared:True" << "\n";

        return true;
    }
    // predPrepared(m, v, n, i), 2 x f + 1 commit(v, n, d) from others
    bool PBFTViewMsgHolder::predCommittedLocal(unsigned int n) {
        std::cout << "predCommittedLocal\n";
        auto ppr_it = _preprepared.find(n);
        if (ppr_it == _preprepared.end()) {
            // does not know m
            std::cout << "predCommittedLocal::does not know m\n";
            return false;
        }
        auto m = ppr_it->second;
        std::string d = PBFTDigest(m->GetBlockHash().str());

        auto pr_it = _prepared.find(n);
        if (pr_it == _prepared.end()) {
            std::cout << "predCommittedLocal::pr_it not found\n";
            return false;
        }
        auto prd_it = pr_it->second->find(d);
        if (prd_it == pr_it->second->end()) {
            std::cout << "predCommittedLocal::prd_it not found\n";
            return false;
        }
        auto prds = prd_it->second;
        if (prds->size() < 2 * _f) {
            std::cout << "predCommittedLocal::prepare count does not match to _f:" << (2*_f) << "\n";
            return false;
        }

        auto cm_it = _committed.find(n);
        if (cm_it == _committed.end()) {
            std::cout << "predCommittedLocal::cm_it not found\n";
            return false;
        }
        auto cmd_it = cm_it->second->find(d);
        if (cmd_it == cm_it->second->end()) {
            std::cout << "predCommittedLocal::cmd_it not found\n";
            return false;
        }
        auto cmds = cmd_it->second;
        if (cmds->size() < 2 * _f + 1) {
            std::cout << "predCommittedLocal::commit count does not match to _f:" << (2*_f + 1) << "\n";
            return false;
        }

        std::cout << "Result_commit:True" << "\n";

        return true;
    }
    std::shared_ptr<PBFTBlock> PBFTViewMsgHolder::getMessage(unsigned int n) {
        auto it = _preprepared.find(n);
        if (it == _preprepared.end()) {
            return nullptr;
        }
        return it->second;
    }


    int PBFTMsgHolder::addPreprepared(unsigned long v, unsigned int n, std::shared_ptr<PBFTBlock> m) {
        auto it = viewMaps.find(v);
        if (it == viewMaps.end()) {
            std::shared_ptr<PBFTViewMsgHolder> sequenceMap = std::make_shared<PBFTViewMsgHolder>(_f);
            sequenceMap->addPreprepared(n, m);
            viewMaps.insert({ v, sequenceMap });
            return 1;
        } else {
            return it->second->addPreprepared(n, m);
        }
    }
    int PBFTMsgHolder::addPrepared(unsigned long v, unsigned int n, std::string d, unsigned long i) {
        std::cout<< "Prepared added for (" << v << ", " << n << ", " << i << ")\n";
        auto it = viewMaps.find(v);
        if (it == viewMaps.end()) {
            std::cout << "Debug PBFTMsgHolder::addPrepared::no it\n";
            std::shared_ptr<PBFTViewMsgHolder> sequenceMap = std::make_shared<PBFTViewMsgHolder>(_f);
            sequenceMap->addPrepared(n, d, i);
            viewMaps.insert({ v, sequenceMap });
            return 1;
        }
        else {
            return it->second->addPrepared(n, d, i);
        }
    }
    int PBFTMsgHolder::addCommit(unsigned long v, unsigned int n, std::string d, unsigned long i) {
        auto it = viewMaps.find(v);
        if (it == viewMaps.end()) {
            std::shared_ptr<PBFTViewMsgHolder> sequenceMap = std::make_shared<PBFTViewMsgHolder>(_f);
            sequenceMap->addCommit(n, d, i);
            viewMaps.insert({ v, sequenceMap });
            return 1;
        }
        else {
            return it->second->addCommit(n, d, i);
        }
    }

    bool PBFTMsgHolder::hasPreprepared(unsigned long v, unsigned int n) {
        auto it = viewMaps.find(v);
        if (it != viewMaps.end()) {
            return it->second->hasPreprepared(n);
        }
        else {
            return false;
        }
    }
    bool PBFTMsgHolder::hasCommit(unsigned long v, unsigned int n, std::string d, unsigned long i) {
        auto it = viewMaps.find(v);
        if (it == viewMaps.end()) {
            return false;
        }
        return it->second->hasCommit(n, d, i);
    }

    // know m, preprepare for m with (v, n), 2 x f prepare(v, n, d, ...) from others
    bool PBFTMsgHolder::predPrepared(unsigned long v, unsigned int n) {
        std::cout << "predPrepared\n";
        auto it = viewMaps.find(v);
        if (it == viewMaps.end()) {
            // does not know m
            std::cout << "predPrepared::does not know m\n";
            return false;
        }
        bool res = it->second->predPrepared(n);
        if (res) {
            predPreparedOnceFlag.insert({{v, n}, true});
        }
        return res;
    }
    bool PBFTMsgHolder::predPreparedOnce(unsigned long v, unsigned int n) {
        auto it = predPreparedOnceFlag.find({v, n});
        if (it != predPreparedOnceFlag.end()) {
            return false;
        } else {
            return predPrepared(v, n);
        }
    }
    // predPrepared(m, v, n, i), 2 x f + 1 commit(v, n, d) from others
    bool PBFTMsgHolder::predCommittedLocal(unsigned long v, unsigned int n) {
        auto it = viewMaps.find(v);
        if (it == viewMaps.end()) {
            // does not know m
            return false;
        }
        bool res = it->second->predCommittedLocal(n);
        if (res) {
            predCommittedLocalOnceFlag.insert({{v, n}, true});
        }
        return res;
    }
    bool PBFTMsgHolder::predCommittedLocalOnce(unsigned long v, unsigned int n) {
        auto it = predCommittedLocalOnceFlag.find({v, n});
        if (it != predCommittedLocalOnceFlag.end()) {
            return false;
        } else {
            return predCommittedLocal(v, n);
        }
    }
    std::shared_ptr<PBFTBlock> PBFTMsgHolder::getMessage(unsigned long v, unsigned int n) {
        auto it = viewMaps.find(v);
        if (it == viewMaps.end()) {
            return nullptr;
        }
        return it->second->getMessage(n);
    }
}
