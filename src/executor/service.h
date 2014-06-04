/********************************
 FileName: executor/service.h
 Author:   WangMin
 Date:     2013-08-14
 Version:  0.1
 Description: executor service
*********************************/

#ifndef SRC_EXECUTOR_SERVICE_H
#define SRC_EXECUTOR_SERVICE_H

#include "include/proxy.h"

using std::string;

class ExecutorService : public ExecutorIf {
public:
    int32_t  Helloworld();

    //RS
    bool Exit();

    // task
    bool StartTask(const string& info);

    bool KillTask(const int32_t job_id, const int32_t task_id);

    bool KillTaskForFT(const int32_t job_id, const int32_t task_id);
    
    // vm heartbeat
    bool SendVMHeartbeat(const string& hb_vm_info_ad);

    //update image
    bool UpdateImage(const string& user, const string& name, const int32_t size);
};

#endif
