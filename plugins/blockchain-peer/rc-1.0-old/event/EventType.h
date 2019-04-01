#ifndef EVENT_TYPE_H
#define EVENT_TYPE_H


enum class EventType {
    nullEvent,
        miningCompleteEvent,
        recvSocketConnectionEvent,
        recvSocketDataEvent,
        sendSocketReadyEvent,
        First=miningCompleteEvent,
        Last=sendSocketReadyEvent
};


// check https://stackoverflow.com/a/8498356 for iteration over EventType



#endif
