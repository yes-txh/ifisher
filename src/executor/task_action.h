/********************************
 FileName: executor/task_action.h
 Author:   Tang Xuehai
 Date:     2014-05-28
 Version:  0.1
 Description: task action
*********************************/

#ifndef SRC_EXECUTOR_TASKACTION_H
#define SRC_EXECUTOR_TASKACTION_H

#include <string>
#include <boost/shared_ptr.hpp>

// #include "include/proxy.h"
#include "include/type.h"
#include "common/clynn/rwlock.h"
#include "common/clynn/singleton.h"

class TaskAction {
public:
    // call back

    void TaskRunning(TaskID id);

    void TaskFinished(TaskID id);

    void TaskMissed(TaskID id);

    void TaskFailed(TaskID id);

    void TaskTimeout(TaskID id);

};

typedef Singleton<TaskAction> TaskActionI;

#endif  
