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
    AccessOrder ac;
    // configuration
    unsigned long int np_filesize = 0x800;
    unsigned int clean_activation = 2;
    unsigned int clean_until = 2;

    int checkReadOnly(const char* modes);
    int cleanup_condition();
    void do_swapouts();
    std::shared_ptr<PersistentElement> swapout(NonPersistentElement* elem_ptr);
    std::shared_ptr<NonPersistentElement> swapin(std::shared_ptr<PersistentElement> elem_ptr);
public:
    FILE* open(const char* filename, const char* modes);
    int close(FILE* file);
    void debug_stats();
    void config(unsigned long int np_filesize, unsigned int clean_activation, unsigned int clean_until) {
        this->np_filesize = np_filesize;
        this->clean_activation = clean_activation;
        this->clean_until = clean_until;
    }
    int debug_fileloc(const char* filename);
};

// Opener
namespace CanDDaGae {
    void config(unsigned long int np_filesize, unsigned int clean_activation, unsigned int clean_until);
    FILE* fopen(const char* filename, const char* modes);
    int fclose(FILE* file);
    void debug();
    int debug_fileloc(const char* filename);
}

#endif //SHADOWSTORAGEMANAGEMENT_CANDDAGAE_H
