//
// Created by csrc on 20. 9. 11..
//

#include "CanDDaGae.h"
#include <iostream>

void AccessOrder::push(NonPersistentElement* np) {
    if (head==NULL) {
        head = np;
        tail = np;
    } else {
        np->next = head;
        np->prev = NULL;
        head->prev = np;
        head = np;
    }
    cnt++;
}
NonPersistentElement* AccessOrder::pop() {
    return remove(tail);
}
NonPersistentElement* AccessOrder::remove(NonPersistentElement* np) {
    if(cnt == 0)
        return nullptr;
    NonPersistentElement* prev = np->prev;
    NonPersistentElement* next = np->next;
    if(prev) {
        if(next) {
            prev->next = next;
            next->prev = prev;
        } else {
            prev->next = NULL;
            tail = prev;
        }
    } else {
        if(next) {
            next->prev = NULL;
            head = next;
        } else {
            head = NULL;
            tail = NULL;
        }
    }
    np->next = NULL;
    np->prev = NULL;
    cnt--;
    return np;
}

int ContentFileTracker::checkReadOnly(const char* modes) {
    int idx = 0;
    while(modes[idx] != 0) {
        if (modes[idx]!='r' && modes[idx]!='b')
            return 0;
        idx++;
    }
    return 1;
}
int ContentFileTracker::cleanup_condition() {
    if (ac.size() >= this->clean_activation) {
        return 1;
    }
    return 0;
}
void ContentFileTracker::do_swapouts() {
    while(ac.size() > this->clean_until) {
        swapout(ac.pop());
    }
    return;
}

PersistentElement* ContentFileTracker::swapout(NonPersistentElement* elem) {
    // try to copy to shared file, if exists, change reference
    char* actual_path = (char*)malloc(sizeof(char)*100);
    elem->try_create(actual_path);
    // add persistent storage to the list
    //std::shared_ptr<StorageElement> p_ptr = std::shared_ptr<StorageElement>(new PersistentElement(elem->get_path(), actual_path));
    StorageElement* p_ptr = new PersistentElement(elem->get_path(), actual_path);
    free(actual_path);
    auto it = elems.find(p_ptr->get_path());
    if(it != elems.end()) {
        StorageElement* tmp_ptr = it->second;
        it->second = p_ptr;
        delete tmp_ptr;
    } else {
        //elems.insert({p_ptr->get_path(), p_ptr});
        abort();
    }
}
NonPersistentElement* ContentFileTracker::swapin(PersistentElement* elem_ptr) {
    // copy file
    long int readcnt = 0;
    long int size = 0;
    NonPersistentElement* np_element = new NonPersistentElement(elem_ptr->get_path(), this->np_filesize);
    FILE* fp = elem_ptr->request("rb");
    while((readcnt = fread(np_element->get_data()+size, sizeof(char), this->np_filesize-size, fp)) != EOF) {
        size += readcnt;
        if(feof(fp))
            break;
    }
    np_element->set_filesize(size);
    fclose(fp);

    // shadow refdown on shared file
    elem_ptr->try_delete();

    // add non-persistent storage to list
    //auto elem = std::shared_ptr<NonPersistentElement>(np_element);
    auto it = elems.find(np_element->get_path());
    if(it != elems.end()) {
        StorageElement* tmp_ptr = it->second;
        it->second = np_element;
        delete tmp_ptr;
    } else {
        //elems.insert({elem->get_path(), elem});
        abort();
    }
    ac.push(np_element);
    if (cleanup_condition()) {
        do_swapouts();
    }
    return np_element;
}
FILE* ContentFileTracker::open(const char* filename, const char* modes) {
    if(!openAccessLock.try_lock() || singleThreadAccessLocked) {
        throw std::runtime_error("CanDDaGae::fopen called twice on same time without CanDDaGae::fclose called");
    }
    singleThreadAccessLocked = true;

    int readOnly = checkReadOnly(modes);
    auto it = elems.find(filename);
    FILE* res;
    if (it != elems.end()) {
        if (it->second->get_storage_type()) {
            // fopen
            res = it->second->request(modes);
            // change access history
            //NonPersistentElement* np = ac.remove(std::dynamic_pointer_cast<NonPersistentElement>(it->second).get());
            NonPersistentElement* np = ac.remove((NonPersistentElement*)it->second);
            ac.push(np);
            // add filename lookup
            np_file_lookup.insert({res, filename});
        } else if(readOnly){
            // Persistent element's request function should be called only in this condition
            res = it->second->request(modes);
        } else {
            //std::shared_ptr<PersistentElement> pe_ptr = std::dynamic_pointer_cast<PersistentElement>(it->second);
            //std::shared_ptr<StorageElement> elem_ptr = std::dynamic_pointer_cast<StorageElement>(swapin(pe_ptr));
            StorageElement* elem_ptr = swapin((PersistentElement*)it->second);
            res = elem_ptr->request(modes);
            np_file_lookup.insert({res, filename});
        }
    } else if(readOnly) {
        errno = ENOENT;
        res = NULL;
    } else {
        NonPersistentElement* np_element = new NonPersistentElement(filename, this->np_filesize);
        auto it = elems.insert({filename, np_element});
        ac.push(np_element);
        if (cleanup_condition()) {
            do_swapouts();
        }
        res = it.first->second->request(modes);
        np_file_lookup.insert({res, filename});
    }
    if (!res) {
        singleThreadAccessLocked = false;
        openAccessLock.unlock();
    }
    return res;
}
long int findFilesize(FILE* file) {
    fseek(file, 0L, SEEK_END);
    long int res = ftell(file);
    return res;
}
int ContentFileTracker::close(FILE* file) {
    auto np_it = np_file_lookup.find(file);
    if(np_it != np_file_lookup.end()) {
        std::string filename = np_it->second;
        auto it = elems.find(filename);
        fflush(file);
        //(std::dynamic_pointer_cast<NonPersistentElement>(it->second))->set_filesize(findFilesize(file));
        ((NonPersistentElement*)(it->second))->set_filesize(findFilesize(file));
        np_file_lookup.erase(file);
    }
    fclose(file);

    singleThreadAccessLocked = false;
    openAccessLock.unlock();
}

// TODO EXT: make debug print
void ContentFileTracker::debug_stats() {
    std::cout<<"element count: "<<elems.size()<<"\n";
    std::cout<<"--element lists--\n";
    auto it = elems.begin();
    while(it != elems.end()) {
        std::cout<<it->first<<"\t||";
        it->second->debug_stat();
        std::cout<<"\n";
        it++;
    }
    std::cout<<"--np_filelookup--\n";
    auto it2 = np_file_lookup.begin();
    while(it2 != np_file_lookup.end()) {
        std::cout<<it2->first<<"\t||"<<it2->second<<"\n";
        it2++;
    }
}
int ContentFileTracker::debug_fileloc(const char* filename){
    auto it = elems.find(filename);
    if (it != elems.end()) {
        return it->second->get_storage_type();
    } else {
        return -1;
    }
}

ContentFileTracker tracker;
void CanDDaGae::config(unsigned long int np_filesize, unsigned int clean_activation, unsigned int clean_until) {
    tracker.config(np_filesize, clean_activation, clean_until);
}
FILE* CanDDaGae::fopen(const char* filename, const char* modes) {
    return tracker.open(filename, modes);
}
int CanDDaGae::fclose(FILE* file) {
    return tracker.close(file);
}
void CanDDaGae::debug() {
    tracker.debug_stats();
}
int CanDDaGae::debug_fileloc(const char* filename) {
    tracker.debug_fileloc(filename);
}