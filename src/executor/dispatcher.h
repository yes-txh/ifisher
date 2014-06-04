/********************************
 FileName: executor/dispatcher.h
 Author:   WangMin
 Date:     2013-09-24
 Version:  0.1
 Description: dispatcher for events; events -> dispatcher -> handler
*********************************/

#ifndef SRC_EXECUTOR_DISPATCHER_H
#define SRC_EXECUTOR_DISPATCHER_H

#include <map>
#include "common/clynn/singleton.h"
#include "executor/handler.h"

using std::map;

class Dispatcher {
public:
    void Register(EventType::type type, Handler* handler);
    Handler* Dispatch(EventType::type type);
private:
    map<EventType::type, Handler*> m_handler_map;
};

typedef Singleton<Dispatcher> EventDispatcherI;

#endif
