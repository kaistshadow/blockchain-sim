//
// Created by csrc on 20. 9. 11..
//

#include "StorageElement.h"
#include "shadow_interface.h"

// This should not be called. Write request should always called on Non-persistent storage element.
FILE* PersistentElement::request(const char* modes) {
    return ::fopen(actual_path.c_str(), modes);
}
// TODO: shadow refdown on shared file => eventually file can be deleted
void PersistentElement::try_delete() {
    shadow_shared_try_delete(this->get_path().c_str());
}

// TODO: shadow refdown on shared file => eventually file can be added
void NonPersistentElement::try_create(char** actual_path_ref) {
    shadow_shared_try_create(this->get_path().c_str(), this->data, this->size, actual_path_ref);
}