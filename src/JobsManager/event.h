/********************************
 FileName: JobsManager/event.h
 Author:   Tangxuehai
 Date:     2014-03-20
 Version:  0.1
 Description: event, and its handler
*********************************/

#ifndef SRC_JOBS_MANAGER_EVENT_H
#define SRC_JOBS_MANAGER_EVENT_H

#include <string>
#include <boost/shared_ptr.hpp>

#include "common/clynn/block_queue.h"
#include "JobsManager/job_enum.h"

using std::string;
using boost::shared_ptr;
using clynn::BlockQueue;

class Event {
public:
    Event(JMEventType type) : m_type(type) {}
    virtual ~Event() {}
    virtual bool Handle() = 0;
    JMEventType GetType() const {
        return m_type;
    }
private:
    JMEventType m_type;
};

class ImageEvent : public Event {
public:
    ImageEvent(const string& user, const string& name, const int32_t size, bool is_update_all) : Event(JMEventType::IMAGE_EVENT),
             m_name(name), m_user(user), m_size(size), m_is_update_all(is_update_all) {}
    
    ~ImageEvent() {}
    bool Handle();
    string GetName() const {
        return m_name;
    }
    string GetUser() const {
        return m_user;
    }
    int32_t GetSize() const {
        return m_size;
    }
    bool GetIs_update_all() const {
	return m_is_update_all;
    }
private:
    string m_name;
    string m_user;
    int32_t m_size;
    bool m_is_update_all;
};


class AppEvent : public Event {
public:
    AppEvent(const string& user, const string& name) : Event(JMEventType::APP_EVENT),
             m_name(name), m_user(user) {}

    ~AppEvent() {}
    bool Handle();
    string GetName() const {
        return m_name;
    }
    string GetUser() const {
        return m_user;
    }
private:
    string m_name;
    string m_user;
};

class TaskEvent : public Event {
public:
    explicit TaskEvent(int32_t job_id, int32_t task_id, JMEventType type) : Event(type), m_job_id(job_id), m_task_id(task_id) {}
    virtual ~TaskEvent() {}
protected:
    int32_t m_job_id;
    int32_t m_task_id;
};

class TaskStateEvent : public TaskEvent {
public:
    explicit TaskStateEvent(int32_t job_id, int32_t task_id) : TaskEvent(job_id, task_id, JMEventType::TASK_STATE_EVENT) {}
    virtual ~TaskStateEvent() {}
};

class TaskActionEvent : public TaskEvent {
public:
    explicit TaskActionEvent(int32_t job_id, int32_t task_id) : TaskEvent(job_id, task_id, JMEventType::TASK_ACTION_EVENT) {}
    virtual ~TaskActionEvent() {}
};

class StartTaskEvent : public TaskActionEvent {
public:
    StartTaskEvent(int32_t job_id, int32_t task_id, const string& endpoint, const string& taskad) 
        : TaskActionEvent(job_id, task_id), m_executor_endpoint(endpoint), m_taskad(taskad) {}
    bool Handle();
private:
    string m_executor_endpoint;
    string m_taskad;
};

class TaskStartedEvent : public TaskStateEvent {
public:
    TaskStartedEvent(int32_t job_id, int32_t task_id) : TaskStateEvent(job_id, task_id) {}
    bool Handle();
};

class TaskFinishedEvent : public TaskStateEvent {
public:
    TaskFinishedEvent(int32_t job_id, int32_t task_id) : TaskStateEvent(job_id, task_id) {}
    bool Handle();
};

class TaskFailedEvent : public TaskStateEvent {
public:
    TaskFailedEvent(int32_t job_id, int32_t task_id) : TaskStateEvent(job_id, task_id) {}
    bool Handle();
};

class TaskMissedEvent : public TaskStateEvent {
public:
    TaskMissedEvent(int32_t job_id, int32_t task_id) : TaskStateEvent(job_id, task_id) {}
    bool Handle();
};

class TaskTimeoutEvent : public TaskStateEvent {
public:
    TaskTimeoutEvent(int32_t job_id, int32_t task_id) : TaskStateEvent(job_id, task_id) {}
    bool Handle();
};

typedef shared_ptr<Event> EventPtr;
typedef BlockQueue<EventPtr> EventQueue;

#endif
