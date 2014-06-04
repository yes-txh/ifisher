/********************************
 FileName: ResourceScheduler/dispatcher.cpp
 Author:   TangXuehai
 Date:     2014-05-11
 Version:  0.1
 Description: dispatcher for events; events -> dispatcher -> handler
*********************************/

#include "ResourceScheduler/dispatcher.h"

// register <EventType::type, Handler*>
void Dispatcher::Register(RSEventType::type type, Handler* handler) {
    m_handler_map[type] = handler;
}

Handler* Dispatcher::Dispatch(RSEventType::type type) {
    return m_handler_map[type];
}
