/********************************
 FileName: JobsManager/JobsManager_service.h
 Author:   ZhangZhang
 Date:     2013-08-19
 Version:  0.1
 Description: JobsManager service
*********************************/

#ifndef SRC_JOBS_MANAGER_SERVICE_H
#define SRC_JOBS_MANAGER_SERVICE_H

#include <vector>
#include "proxy/JobsManager/gen-cpp/JobsManager.h"

using std::string;
using std::vector;

class JobsManagerService : public JobsManagerIf {
public:
    void SubmitJob(SubmitJobResult& result, const string& job_xml);
    void GetJobId(vector<int32_t>& job_id_list);
    void GetTaskStateInfo(vector<TaskStateInfo>& task_state_info_list, int32_t job_id);
    int32_t GetWaitJobNumByGroupName(const string& group);
    void GetGroupStateInfo(GroupStateInfo& group_info, const string& group_name); 

    bool TaskStarted(int32_t job_id, int32_t task_id);
    bool TaskFinished(int32_t job_id, int32_t task_id);
    bool TaskFailed(int32_t job_id, int32_t task_id);
    bool TaskMissed(int32_t job_id, int32_t task_id);
    bool TaskTimeout(int32_t job_id, int32_t task_id);
    bool TaskStartError(int job_id, int task_id, int err_num);

    bool UpdateImage(const string& user, const string& name, int size, bool is_update_all);
    bool CreateApp(const string& user, const string& name);
};

#endif
