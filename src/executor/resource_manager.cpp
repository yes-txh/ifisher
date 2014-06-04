/********************************
 FileName: executor/resource_manager.cpp
 Author:   WangMin
 Date:     2013-08-27
 Version:  0.1
 Description: resource manager of the machine(execute node)
*********************************/

#include <stdlib.h>
#include <iostream>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <gflags/gflags.h>
#include <classad/classad.h>
#include <classad/classad_distribution.h>

#include "common/classad/classad_complement.h"
#include "include/classad_attr.h"
#include "include/type.h"
#include "executor/system.h"
#include "executor/vm_pool.h"
#include "executor/image_manager.h"
#include "executor/resource_manager.h"

using std::cout;
using std::endl;
using std::stringstream;
using log4cplus::Logger;
using clynn::ReadLocker;
using clynn::WriteLocker;

DECLARE_int32(port);
DECLARE_string(resource_scheduler_endpoint);
DECLARE_string(interface);
DECLARE_string(if_bridge);
DECLARE_string(lynn_version);


static Logger logger = Logger::getInstance("executor");

// init, set static info 
// TODO
bool ResourceManager::Init() {
    /// @brief: set private info
    // ip
    m_ip = System::GetIP(FLAGS_interface.c_str());
    m_bridge_ip = System::GetIP(FLAGS_if_bridge.c_str());
    m_port = FLAGS_port;

    // machine & arch
    m_name = m_ip;
    m_machine_type = "A";
    m_shelf_number = -1;
    m_arch = "Intel";
    m_os = System::GetOSVersion();

    m_total_cpu = System::GetCpuNum();
    m_total_memory = System::GetTotalMemory(); 
    m_total_disk = System::GetTotalDisk();
    m_avail_cpu = m_total_cpu;
    m_avail_memory = m_total_memory;
    m_avail_disk = m_total_disk - System::GetUsedDisk();

    m_band_width = System::GetBandWidth(FLAGS_interface.c_str());
    m_nic_type = System::GetNICType(FLAGS_interface.c_str());

    /// @brief: get info, send info to collector
    string machine_info = GetMachineInfo();
    // TODO
    cout << machine_info << endl; 
  
     
    // VMPool Init check libvirt dir and lxc dir
    if (!VMPoolI::Instance()->Init()) {
        LOG4CPLUS_ERROR(logger, "error in VMPool Init.");
        return false;
    }
    return true;
}

string ResourceManager::GetMachineInfo() {
    ReadLocker lock(m_lock);
    
    // init classad
    ClassAd ad;
    ad.InsertAttr(ATTR_Machine, m_name);
    ad.InsertAttr(ATTR_MachineType, m_machine_type);
    ad.InsertAttr(ATTR_Shelf, m_shelf_number);
    ad.InsertAttr(ATTR_Machine_IP, m_ip);
    ad.InsertAttr(ATTR_Port, m_port);
    ad.InsertAttr(ATTR_Arch, m_arch);
    ad.InsertAttr(ATTR_OpSys, m_os);
    ad.InsertAttr(ATTR_TotalCPUNum, m_total_cpu);
    ad.InsertAttr(ATTR_TotalMemory, m_total_memory);
    ad.InsertAttr(ATTR_TotalDisk, m_total_disk);
    ad.InsertAttr(ATTR_BandWidth, m_band_width);
    ad.InsertAttr(ATTR_NICType, m_nic_type);
  
    // classad -> string
    ClassAdUnParser unparser;
    string str_ad;
    // Serialization, convert ClassAd into string str_ad
    unparser.Unparse(str_ad, &ad);
    
    return str_ad; 
}

string ResourceManager::GetBridgeEndpoint() {
    stringstream ss;
    ss << m_port;
    return m_bridge_ip + ":" + ss.str();
}

void ResourceManager::GenerateHb(string& machine_str_ad, vector<string>& task_list) {
    HbMachineInfo hb_m_info;
    // node machine
    hb_m_info.ip = m_ip;
    hb_m_info.port = m_port;
    // m_total_cpu = System::GetCpuNum();
    // m_total_memory = System::GetTotalMemory();
    // m_total_disk = System::GetTotalDisk();
    m_loadavg = System::GetLoadAvginFive();
    m_avail_cpu = m_total_cpu - VMPoolI::Instance()->GetAllocatedCPU();
    m_avail_memory = m_total_memory - VMPoolI::Instance()->GetAllocatedMemory();
    m_avail_disk = m_total_disk - System::GetUsedDisk();
    // image attr
    m_image_attr = ImageMgrI::Instance()->GetImageAttrFromFile();
	
    hb_m_info.cpu_usage = System::GetCpuUsage();
    hb_m_info.memory_usage = System::GetMemoryUsage();
    System::GetNetFlowinBytes(FLAGS_interface.c_str(), hb_m_info.bytes_in, hb_m_info.bytes_out);

    cout << "-------resource_manager.cpp------" << endl;
    cout << "Heartbeat:" << endl;
    cout << "--ip: " << hb_m_info.ip << endl;
    cout << "--port: " << hb_m_info.port << endl;
    cout << "--avail_cpu: " << m_avail_cpu << endl;
    cout << "--avail_memory: " << m_avail_memory << endl;
    cout << "--avail_disk: " << m_avail_disk << endl;
    cout << "--image_attr: " << m_image_attr << endl;
    cout << "--cpu_usage: " << hb_m_info.cpu_usage << endl;
    cout << "--memory_usage: " << hb_m_info.memory_usage << endl;
    cout << "--bytes_in: " << hb_m_info.bytes_in << endl;
    cout << "--bytes_out: " << hb_m_info.bytes_out << endl;
    cout << "VMHeartbeatList:" << endl;

    // classad
    ClassAd ad;
    ad.InsertAttr(ATTR_Machine, m_name);
    ad.InsertAttr(ATTR_MachineType, m_machine_type);
    ad.InsertAttr(ATTR_Shelf, m_shelf_number);
    ad.InsertAttr(ATTR_Machine_IP, m_ip);
    ad.InsertAttr(ATTR_Port, m_port);
    ad.InsertAttr(ATTR_Arch, m_arch);
    ad.InsertAttr(ATTR_OpSys, m_os);
    ad.InsertAttr(ATTR_LOAD_AVG, m_loadavg);
    ad.InsertAttr(ATTR_TotalCPUNum, m_total_cpu);
    ad.InsertAttr(ATTR_TotalMemory, m_total_memory);
    ad.InsertAttr(ATTR_TotalDisk, m_total_disk);
    ad.InsertAttr(ATTR_AvailCPUNum, m_avail_cpu);
    ad.InsertAttr(ATTR_AvailMemory, m_avail_memory);
    ad.InsertAttr(ATTR_AvailDisk, m_avail_disk);
    ad.InsertAttr(ATTR_BandWidth, m_band_width);
    ad.InsertAttr(ATTR_NICType, m_nic_type);
    ad.InsertAttr(ATTR_Image, m_image_attr);

    ClassAdUnParser unparser;
    // Serialization, convert ClassAd into string str_ad
    unparser.Unparse(machine_str_ad, &ad);
    

    // vm list
    hb_m_info.hb_vminfo_list = VMPoolI::Instance()->GetAllHbVMInfo();
    vector<HbVMInfo> vm_list = hb_m_info.hb_vminfo_list;
    for (vector<HbVMInfo>::iterator it = vm_list.begin();
         it != vm_list.end(); ++it) {
        cout << "--VM:" << endl;
        cout << "----job_id: " << (it->id).job_id << endl;
        cout << "----task_id: " << (it->id).task_id << endl;
        //cout << "----name: " << it->name << endl;
        //cout << "----type: " << it->type << endl;
        cout << "----app_state: " << it->app_state << endl;
        cout << "----cpu_usage: " << it->cpu_usage << endl;
        cout << "----memory_usage: " << it->memory_usage << endl;
        cout << "----bytes_in: " << it->bytes_in << endl;
        cout << "----bytes_out: " << it->bytes_out << endl;

        ClassAd task_ad;
        task_ad.InsertAttr(ATTR_JOB_ID_RS, (it->id).job_id);
        task_ad.InsertAttr(ATTR_TASK_ID_RS, (it->id).task_id);
        task_ad.InsertAttr(ATTR_VMHB_CPU, it->cpu_usage);
        task_ad.InsertAttr(ATTR_VMHB_MEMORY, it->memory_usage);
        task_ad.InsertAttr(ATTR_VMHB_BYTES_IN, it->bytes_in);
        task_ad.InsertAttr(ATTR_VMHB_BYTES_OUT, it->bytes_out);
        task_ad.InsertAttr(ATTR_VMHB_STATE, it->app_state);
    
        ClassAdUnParser unparser;
        string task_str_ad;
        unparser.Unparse(task_str_ad, &task_ad);
        task_list.push_back(task_str_ad);
    }
}


void ResourceManager::AllocateResource(double cpu, int32_t memory, int32_t disk) {
    WriteLocker locker(m_lock);
    m_avail_cpu -= cpu;
    m_avail_memory -= memory;
    m_avail_disk -= disk;
}

void ResourceManager::ReleaseResource(double cpu, int32_t memory, int32_t disk) {
    WriteLocker locker(m_lock);
    m_avail_cpu += cpu;
    m_avail_memory += memory;
    m_avail_disk += disk;
}
