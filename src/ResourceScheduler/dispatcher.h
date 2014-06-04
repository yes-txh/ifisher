/********************************
 FileName: ResourceScheduler/dispatcher.h
 Author:   TangXuehai
 Date:     2014-05-12
 Version:  0.1
 Description: dispatcher for events; events -> dispatcher -> handler
*********************************/

#ifndef SRC_RESOURCE_SCHEDULER_DISPATCHER_H
#define SRC_RESOURCE_SCHEDULER_DISPATCHER_H

#include <map>
#include "common/clynn/singleton.h"
#include "ResourceScheduler/handler.h"

using std::map;

class Dispatcher {
public:
    void Register(RSEventType::type type, Handler* handler);
    Handler* Dispatch(RSEventType::type type);
private:
    map<RSEventType::type, Handler*> m_handler_map;
};

typedef Singleton<Dispatcher> EventDispatcherI;

#endif
