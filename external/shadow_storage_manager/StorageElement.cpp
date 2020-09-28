//
// Created by csrc on 20. 9. 11..
//

#include "StorageElement.h"
#include "shadow_interface.h"
#include <iostream>

void StorageElement::debug_stat() {
	std::cout<<this->path<<"\t";
	if(this->storage_type) {
		std::cout<<"MEM\t";
	} else {
		std::cout<<"DISK\t";
	}
}

FILE* PersistentElement::request(const char* modes) {
    return ::fopen(actual_path.c_str(), modes);
}
// TODO: shadow refdown on shared file => eventually file can be deleted
void PersistentElement::try_delete() {
//    shadow_shared_try_delete(this->get_actual_path().c_str());
}
void PersistentElement::debug_stat() {
    StorageElement::debug_stat();
	std::cout<<"Actual path: "<<this->actual_path;
}

// TODO: shadow refdown on shared file => eventually file can be added
void NonPersistentElement::try_create(char** actual_path_ref) {
    shadow_shared_try_create(this->get_path().c_str(), this->data, this->size, actual_path_ref);
}
void NonPersistentElement::debug_stat() {
    StorageElement::debug_stat();
	std::cout<<"Max Buffer Size: "<<this->filesize_limit<<"\t Current File Size: "<<this->size;
}