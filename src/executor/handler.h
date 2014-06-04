/********************************
 FileName: executor/handler.h
 Author:   WangMin
 Date:     2013-09-24
 Version:  0.1
 Description: handler for event
*********************************/

#ifndef SRC_EXECUTOR_HANDLER_H
#define SRC_EXECUTOR_HANDLER_H

#include <queue>
#include "common/clynn/thread.h"
#include "executor/event.h"

using std::queue;
using clynn::Thread;

class Handler {
public:
    Handler() : m_work_thread(0) {}
    ~Handler();
   
    // @brief: begin to work
    void Start();

    void Handle();
    void PushBack(const EventPtr& ptr);

private:
    // BlockQueue of EventPtr
    EventQueue m_event_queue;
    clynn::Thread* m_work_thread;
};

#endif
