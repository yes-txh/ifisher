/********************************
 FileName: executor/task_entity_pool.h
 Author:   WangMin
 Date:     2013-08-27
 Version:  0.1
 Description: the pool of taskentitys
*********************************/

#ifndef SRC_EXECUTOR_TASKENTITY_POOL_H
#define SRC_EXECUTOR_TASKENTITY_POOL_H

#include <map>
// tr1 is lib of c++ std
#include <tr1/memory>
#include <tr1/functional>

#include "common/clynn/rwlock.h"
#include "common/clynn/singleton.h"
#include "executor/task_entity.h"

using std::map;
using std::tr1::function;         // similar to a function pointer;
using std::tr1::placeholders::_1; // 
using std::tr1::placeholders::_2; 
using clynn::RWLock;

class TaskEntityPool {
public:
    // function pointer, from std::tr1::function
    typedef function<void(TaskEntity*)> TaskFunc;

    // TODO print all
    void PrintAll();

    // @brief: find a taskentity in map
    bool Find(const TaskPtr& ptr);

    // @brief: insert a taskentity in map
    void Insert(const TaskPtr& ptr);

    // @brief: delete a taskentity from map
    void Delete(const TaskID id);

    bool InsertIfAbsent(const TaskPtr& ptr);

    bool FindToDo(const TaskID id, TaskFunc func);

    // @brief: find a waiting taskentity, and start it
    void StartTask();

    bool StartTaskByID(const TaskID id);

    bool StopTaskByID(const TaskID id);

    bool KillTaskByID(const TaskID id);

    TaskPtr GetTaskPtr(const TaskID id);

private:
    // <TaskID(job_id, task_id), executor pointer>
    map<TaskID, TaskPtr> m_task_map;
    RWLock m_lock;
};

typedef Singleton<TaskEntityPool> TaskPoolI;

#endif
