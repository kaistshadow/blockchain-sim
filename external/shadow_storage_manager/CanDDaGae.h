//
// Created by csrc on 20. 9. 11..
//

#ifndef SHADOWSTORAGEMANAGEMENT_CANDDAGAE_H
#define SHADOWSTORAGEMANAGEMENT_CANDDAGAE_H

#include "StorageElement.h"
#include <memory>
#include <unordered_map>

class AccessOrder {
    NonPersistentElement *head, *tail;
    int cnt;
public:
    AccessOrder() {
        head = nullptr;
        tail = nullptr;
        cnt = 0;
    }
    void push(NonPersistentElement* np);
    NonPersistentElement* pop();
    NonPersistentElement* remove(NonPersistentElement* np);
    int size() const {return cnt;}
};

class ContentFileTracker {
private:
    std::unordered_map<std::string, std::shared_ptr<StorageElement>> elems;
    std::unordered_map<FILE*, std::string> np_file_lookup;
#define CLEAN_ACTIVATION_THRESHOLD 2
#define CLEAN_UNTIL_THRESHOLD 2
    AccessOrder ac;

    int checkReadOnly(const char* modes);
    int cleanup_condition();
    void do_swapouts();
    std::shared_ptr<PersistentElement> swapout(NonPersistentElement* elem_ptr);
    std::shared_ptr<NonPersistentElement> swapin(std::shared_ptr<PersistentElement> elem_ptr);
public:
    FILE* open(const char* filename, const char* modes);
    int close(FILE* file);
    void debug_stats();
};

// Opener
namespace CanDDaGae {
    FILE* fopen(const char* filename, const char* modes);
    int fclose(FILE* file);
}

#endif //SHADOWSTORAGEMANAGEMENT_CANDDAGAE_H
