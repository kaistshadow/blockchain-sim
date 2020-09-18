//
// Created by csrc on 20. 9. 11..
//

#ifndef SHADOWSTORAGEMANAGEMENT_STORAGEELEMENT_H
#define SHADOWSTORAGEMANAGEMENT_STORAGEELEMENT_H

#include <string>

#define STORAGE_PERSISTENT 0
#define STORAGE_NONPERSISTENT 1
class StorageElement {
private:
    std::string path;
    int storage_type; // memory: 0, storage: 1
public:
    StorageElement(std::string path, int storage_type) {
        this->path = path;
        this->storage_type = storage_type;
    }
    std::string get_path() {return path;}
    int get_storage_type() {return storage_type;}
    virtual FILE* request(const char* modes) = 0;
};
class PersistentElement : public StorageElement{
private:
    std::string actual_path;
public:
    PersistentElement(std::string path, std::string actual_path) : StorageElement(path, STORAGE_PERSISTENT) {this->actual_path = actual_path;}
    FILE* request(const char* modes);
    std::string get_actual_path() {return actual_path;}
    void try_delete();
};

class NonPersistentElement : public StorageElement{
private:
    char* data;
    long int size;
    int filesize_limit;
public:
    NonPersistentElement *prev, *next;
    NonPersistentElement(std::string path, int filesize) : StorageElement(path, STORAGE_NONPERSISTENT) {
        this->filesize_limit = filesize;
        data = (char*)malloc(sizeof(char)*filesize);
        prev = NULL;
        next = NULL;
    }
    ~NonPersistentElement() {
        free(data);
    }
    FILE* request(const char* modes) {
        return fmemopen(data, filesize_limit, modes);
    }
    void set_filesize(long int size) {this->size = size;}
    char* get_data() {
        return this->data;
    }
    void try_create(char** actual_path_ref);
};
#endif //SHADOWSTORAGEMANAGEMENT_STORAGEELEMENT_H
