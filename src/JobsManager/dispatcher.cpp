/********************************
 FileName: JobsManager/dispatcher.cpp
 Author:   Tangxuehai
 Date:     2014-03-20
 Version:  0.1
 Description: dispatcher for events; events -> dispatcher -> handler
*********************************/

#include "JobsManager/dispatcher.h"

// register <JMEventType, Handler*>
void Dispatcher::Register(JMEventType type, Handler* handler) {
    m_handler_map[type] = handler;
}

Handler* Dispatcher::Dispatch(JMEventType type) {
    return m_handler_map[type];
}
