/********************************
 FileName: resourcescheduler/event.h
 Author:   TangXuehai
 Date:     2014-05-13
 Version:  0.1
 Description: event, and its handler
*********************************/

#ifndef SRC_RESOURCE_SCHEDULER_EVENT_H
#define SRC_RESOURCE_SCHEDULER_EVENT_H

#include <string>
#include <boost/shared_ptr.hpp>

#include "common/clynn/block_queue.h"

#include "include/proxy.h"
#include "include/type.h"

using std::string;
using boost::shared_ptr;
using clynn::BlockQueue;

class Event {
public:
    Event(RSEventType::type type) : m_type(type)  {}
    virtual ~Event() {}
    virtual bool Handle() = 0;
    RSEventType::type GetType() const {
        return m_type;
    }
private:
    RSEventType::type m_type;
};



class MachineUpdateEvent : public Event {
public:
    MachineUpdateEvent(const string& heartbeat_ad, const vector<string>& task_list) : Event(RSEventType::MACHINE_UPDATE_EVENT),
             m_heartbeat_ad(heartbeat_ad), m_task_list(task_list) {}

    ~MachineUpdateEvent() {}
    bool Handle();
    string GetHeartbeatAd() const {
        return m_heartbeat_ad;
    }
    vector<string> GetTaskList() const {
        return m_task_list;
    }
private:
    string m_heartbeat_ad;
    vector<string>  m_task_list;
};


class MachineDeleteEvent : public Event {
public:
    MachineDeleteEvent(const string& ip) : Event(RSEventType::MACHINE_DELETE_EVENT),
             m_ip(ip) {}

    ~MachineDeleteEvent() {}
    bool Handle();
    string GetIp() const {
        return m_ip;
    }
private:
    string m_ip;
};


typedef shared_ptr<Event> EventPtr;
typedef BlockQueue<EventPtr> EventQueue;

#endif
