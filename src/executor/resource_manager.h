/********************************
 FileName: executor/resource_manager.h
 Author:   WangMin
 Date:     2013-08-27
 Version:  0.1
 Description: resource manager of the machine(execute node)
*********************************/

#ifndef SRC_EXECUTOR_RESOURCE_MANAGER_H
#define SRC_EXECUTOR_RESOURCE_MANAGER_H

#include <string>
#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>

#include "include/proxy.h"
#include "common/clynn/rpc.h"
#include "common/clynn/singleton.h"
#include "common/clynn/rwlock.h"

using std::string;
using std::map;
using std::vector;
using boost::shared_ptr;
using clynn::RWLock;

class ResourceManager {
public:
    // init, set static info 
    bool Init();

    // get static register info
    string GetMachineInfo();

    string GetBridgeEndpoint();

    void GenerateHb(string& machine_str_ad, vector<string>& task_list);    

    void AllocateResource(double cpu, int32_t memory, int32_t disk);
  
    void ReleaseResource(double cpu, int32_t memory, int32_t disk);
    
private:
    RWLock m_lock;
    // machine & arch
    string m_name;
    string m_machine_type;
    int32_t m_shelf_number;
    string m_arch;
    string m_os;

    // ip 
    string m_ip;        // communicate with master
    string m_bridge_ip; // communicate with vm
    int m_port; 
  
    // resource
    double m_loadavg;
    int32_t m_total_cpu;
    int32_t m_total_memory; // M
    int32_t m_total_disk;   // G
    double m_avail_cpu;
    int32_t m_avail_memory;
    int32_t m_avail_disk;
    int32_t m_band_width;
    string m_nic_type;      // interface type;
    string m_image_attr;    //image attr
};

typedef Singleton<ResourceManager> ResourceMgrI;

#endif 
