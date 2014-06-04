/********************************
 FileName: JobsManager/dispatcher.h
 Author:   Tangxuehai
 Date:     2014-03-20
 Version:  0.1
 Description: dispatcher for events; events -> dispatcher -> handler
*********************************/

#ifndef SRC_EXECUTOR_DISPATCHER_H
#define SRC_EXECUTOR_DISPATCHER_H

#include <map>
#include "common/clynn/singleton.h"
#include "JobsManager/handler.h"

using std::map;

class Dispatcher {
public:
    void Register(JMEventType type, Handler* handler);
    Handler* Dispatch(JMEventType type);
private:
    map<JMEventType, Handler*> m_handler_map;
};

typedef Singleton<Dispatcher> EventDispatcherI;

#endif
