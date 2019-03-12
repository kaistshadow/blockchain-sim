#ifndef FILE_DESCRIPTOR_EVENT_NOTIFIER_H
#define FILE_DESCRIPTOR_EVENT_NOTIFIER_H

#include "FileDescriptorEvent.h"

class FileDescriptorEventNotifier {
 private:
    /* event-triggered fd */
    int _eventTriggeredFD;
    FileDescriptorEventEnum _eventType;
    bool _eventTriggered = false;

 public:
    FileDescriptorEventNotifier() {}

    bool IsEventTriggered() { return _eventTriggered; }
    int GetEventTriggeredFD() { return _eventTriggeredFD; }
    FileDescriptorEventEnum GetEventType() { return _eventType; }

    void SetEventTriggered(bool triggered) { _eventTriggered = triggered; }
    void SetEventTriggeredFD(int fd) { _eventTriggeredFD = fd; }
    void SetEventType(FileDescriptorEventEnum type) { _eventType = type; }

};


#endif
