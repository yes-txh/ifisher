/********************************
 FileName: ResourceScheduler/resource_scheduler_service.cpp
 Author:   ZhangZhang 
 Date:     2014-05-11
 Version:  0.1
 Description: resource scheduler service
*********************************/

#ifndef SRC_RESOURCE_SCHEDULER_SERVICE_H
#define SRC_RESOURCE_SCHEDULER_SERVICE_H

#include <string>
#include <list>
#include <vector>
#include "proxy/ResourceScheduler/gen-cpp/ResourceScheduler.h"

using std::string;
using std::vector;

class ResourceSchedulerService : public ResourceSchedulerIf{
public:
    void GetCurVersion(string& new_version);
    int RegistMachine(const string& ip, const string& version);
    int UpdateMachine(const string& heartbeat_ad);
    int NewUpdateMachine(const string& heartbeat_ad, const vector<string>& task_list);
    int DeleteMachine(const string& ip);
    void GetTotalResource(MultiD_Resource& md_r);
    void MatchMachine(string& result, const string& task_ad);
    void MatchMachineConstraint(string& result, const string& task_ad, const vector<string>& soft_list);
    void QueryMachine(string& result, const string& machine);
    void ListAllMachines(vector<string>& result);
    void Negotiate(vector<string>& result, const string& request);
    void GetMachineListByImageInfo(vector<string>& machine_list, const string& user, const string& name, const int32_t size, bool is_update_all);
};

#endif
