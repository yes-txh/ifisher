/********************************
 FileName: ResourceScheduler/resource_scheduler_engine.h
 Author:   ZhangZhang & Tangxuehai
 Date:     2014-05-11
 Version:  0.1
 Description: resource scheduler engine
*********************************/

#ifndef SRC_RESOURCE_SCHEDULER_ENGINE_H
#define SRC_RESOURCE_SCHEDULER_ENGINE_H

#include <map>
#include <set>
#include <vector>
#include <string>
#include <stdint.h>
#include <tr1/functional>

#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 5) 
#include <atomic>  // for ubuntu
#else
#include <cstdatomic> // for centos
#endif

#include "common/clynn/singleton.h"
#include "common/clynn/rwlock.h"
#include "common/classad/classad_complement.h"
#include "proxy/ResourceScheduler/gen-cpp/ResourceScheduler.h"
#include "ResourceScheduler/machine.h"
#include "ResourceScheduler/machine_constances.h"

using std::map;
using std::string;
using std::set;
using std::vector;
using clynn::RWLock;
using std::tr1::function;
using std::atomic_int;

class ResourceSchedulerEngine {
    public:
        ResourceSchedulerEngine();
        ~ResourceSchedulerEngine();
        int Init();
	void GetCurVersion(string& new_version);
	int RegistMachine(const string& ip, const string& version);
        int UpdateMachine(const string& machine_ad);
	int NewUpdateMachine(const string& machine_ad, const vector<string>& task_list);
        int DeleteMachine(const string& ip);
        // get total resource including cpu, memory, disk. service for JobsManager
        MultiD_Resource GetTotalResource();
	void MachineMonitor();
        int InvokeHousekeeper();
        void  HousekeeperTimer(int *count, uint64_t timer_id);
        bool SetResourceSchedulerRequirements(string str, string& error_desc);
        int CleanMachines(time_t);
        int FetchMachinesForNegotiator(const ClassAdPtr job, vector<ClassAd>& machine_ads); 
        bool FetchMachinesForNegotiatorTask(const ClassAdPtr task, string& machine);
	bool FetchMachinesForNegTaskConst(const ClassAdPtr task, const vector<string>& soft_list, string& machine);
        void GetMachineListByImageInfo(vector<string>& machine_list, const string& user, const string& name, const int32_t size, bool is_update_all);

    private:
        bool GetIdByIp(int& id, const string& ip);
        bool MapIpToId(int& id, const string& ip);
        bool CheckAd(const ClassAd& ad);
	bool TempAllocResource(const ClassAdPtr task, int match_machine_id, const string& machine);
	void UpdateMachineStamp(string ip);
        atomic_int m_cur_id;
        RWLock m_ip_to_id_map_lock;
        map<string, int> m_ip_to_id_map;
	RWLock ip_to_stamp_map_lock;
	map<string, int32_t> ip_to_stamp_map;
        RWLock m_machine_locks[MACHINE_NUM];
        MachinePtr m_machine_pool[MACHINE_NUM];
};

typedef Singleton<ResourceSchedulerEngine> COLLECTOR_ENGINE;
#endif
