//
// Created by csrc on 21. 4. 26..
//

#ifndef BLEEP_SHADOW_MEMSHARE_INTERFACE_H
#define BLEEP_SHADOW_MEMSHARE_INTERFACE_H

#include <typeinfo>
#include <unordered_set>
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <mutex>

#define MEMSHARE_DISABLED

/* sharing target object requires:
 * 1. bool operator==(const A& other)
 * 2. std::size_t hash()
 */

namespace memshare {
    class memory_sharing_unspecified {
    private:
        std::mutex tbl_lock;
    public:
        virtual ~memory_sharing_unspecified() {}
        virtual void try_share(void* sptr_ref);
        virtual void* lookup(void* sptr_ref);
        void lock_tbl();
        void unlock_tbl();
    };

    template <typename SPTR_ELEM>
    class memory_sharing : public memory_sharing_unspecified {
    private:
        struct Hash {
            template <typename T, template <typename ELEM> typename SPTR_TYPE>
            std::size_t operator() (SPTR_TYPE<T> const &p) const {
                return p->hash();
            }
        };
        struct Compare {
            template <typename T, typename U, template <typename ELEM> typename SPTR_TYPE>
            size_t operator() (SPTR_TYPE<T> const &a, SPTR_TYPE<U> const &b) const {
                return typeid(*a) == typeid(*b) && *a == *b;
            }
        };
        std::unordered_set<SPTR_ELEM, Hash, Compare> tbl;
    public:
        memory_sharing() {}
        virtual ~memory_sharing() {}
        void try_share(void* sptr_ref) {
            SPTR_ELEM* sptr_ptr = (SPTR_ELEM*)sptr_ref;
            lock_tbl();
            tbl.insert(*sptr_ptr);
            unlock_tbl();
        }
        void* lookup(void* sptr_ref) {
            SPTR_ELEM* sptr_ptr = (SPTR_ELEM*)sptr_ref;
            lock_tbl();
            auto it = tbl.find(*sptr_ptr);
            unlock_tbl();
            if (it != tbl.end()) {
                SPTR_ELEM* res = new SPTR_ELEM();
                *res = *it;
                return (void *) res;
            }
            return sptr_ref;
        }
    };
}

extern "C"
{
void shadow_try_register_memshare_table(void* type_idx_ref, void* mtbl);
void shadow_memshare_try_share(void* type_idx_ref, void* sptr_ref);
void* shadow_memshare_lookup(void* type_idx_ref, void* sptr_ref);
}

namespace memshare {
    void set_shared_type_cache(std::type_index tidx);
    int check_shared_type_cache(std::type_index tidx);
    template <typename SPTR_TYPE>
    void try_register_table() {
        #ifndef MEMSHARE_DISABLED
        std::type_index type_idx = std::type_index(typeid(SPTR_TYPE));
        memory_sharing_unspecified* mtbl = new memory_sharing<SPTR_TYPE>();
        shadow_try_register_memshare_table(&type_idx, mtbl);
        set_shared_type_cache(type_idx);
        #else
        return;
        #endif
    }
    template <typename SPTR_TYPE>
    void try_share(SPTR_TYPE sptr) {
        #ifndef MEMSHARE_DISABLED
        std::type_index type_idx = std::type_index(typeid(SPTR_TYPE));
        if (check_shared_type_cache(type_idx)) {
            try_register_table<SPTR_TYPE>();
        }
        shadow_memshare_try_share(&type_idx, &sptr);
        #else
        return;
        #endif
    }
    template <typename SPTR_TYPE>
    SPTR_TYPE lookup(SPTR_TYPE sptr) {
        #ifndef MEMSHARE_DISABLED
        std::type_index type_idx = std::type_index(typeid(SPTR_TYPE));
        if (check_shared_type_cache(type_idx)) {
            try_register_table<SPTR_TYPE>();
        }
        SPTR_TYPE* sptr_ptr = (SPTR_TYPE*)shadow_memshare_lookup(&type_idx, &sptr);
        SPTR_TYPE res = *sptr_ptr;
        if (sptr_ptr != &sptr) {
            delete sptr_ptr;
        }
        return res;
        #else
        return sptr;
        #endif
    }
}

#endif //BLEEP_SHADOW_MEMSHARE_INTERFACE_H
