/********************************
 FileName: ResourceScheduler/machine.cpp
 Author:   ZhangZhang & Tangxuehai
 Date:     2014-05-11
 Version:  0.1
 Description: machine 
*********************************/

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>


#include "include/classad_attr.h"
#include "ResourceScheduler/machine.h"


using log4cplus::Logger;
using std::stringstream;
using std::vector;

using clynn::ReadLocker;
using clynn::WriteLocker;

static Logger logger = Logger::getInstance("ResourceScheduler");

Machine::Machine(const ClassAdPtr& machine_ad) {
    m_machine_ad = machine_ad;
    m_id = -1;
}

string Machine::GetMachineEndpoint(){
    return m_endpoint;
}

void Machine::Update(const ClassAdPtr& machine_ad){
    m_machine_ad = machine_ad;   
}

bool Machine::ParseMachineAd() {
    if(m_machine_ad->EvaluateAttrString(ATTR_Machine_IP, m_ip) == false){
        return false;
    } else if (m_machine_ad->EvaluateAttrInt(ATTR_Port, m_port) == false) {
        return false;
    }
    stringstream ss;
    ss << m_ip << ":" << m_port;
    m_endpoint = ss.str();
    return true;
}

bool Machine::GetMachineByImageInfo(const string& user, const string& name, const int32_t size) {
    string image_info_str;
    stringstream size_ss;
    size_ss << size;
    string image_attr = user + "&" + name + "&" + size_ss.str();

    if(m_machine_ad->EvaluateAttrString(ATTR_Image,image_info_str) == false){
        return false;
    }
    if(image_info_str.empty()) {
	return false;
    }
    //for one image_attr
    if(image_info_str.find(":") == string::npos && image_info_str.find("&") != string::npos) {
        if(image_attr == image_info_str) {
            return true;
        } else {
		return false;	
	}

    } else if(image_info_str.find(":") != string::npos && image_info_str.find("&") != string::npos) {
	    vector<string> image_info_list;
	    boost::split(image_info_list, image_info_str, boost::is_any_of( ":" ), boost::token_compress_on);
    	    if(image_info_list.size() == 0) {
                return false;
	    }
	
	    for(vector<string>::iterator it = image_info_list.begin(); it != image_info_list.end(); ++it) {
		if(image_attr == *it) {
	            return true;
        	} else {
                    continue; 
        	}
	    }
    } else {
	return false;
    }
    return false;
}

bool Machine::TempAllocResource(const TaskID& id, const AllocResource& alloc_resource) {
    //update machine classad
    int32_t avail_cpu_num;
    int32_t avail_memory;
    if(m_machine_ad->EvaluateAttrNumber(ATTR_AvailCPUNum, avail_cpu_num) == false){
        return false;
    }

    if(m_machine_ad->EvaluateAttrNumber(ATTR_AvailMemory, avail_memory) == false){
        return false;
    }
    avail_cpu_num = avail_cpu_num - alloc_resource.cpu;
    avail_memory = avail_memory - alloc_resource.memory;
    m_machine_ad->InsertAttr(ATTR_AvailCPUNum, avail_cpu_num);
    m_machine_ad->InsertAttr(ATTR_AvailMemory, avail_memory);

    //record temporary alloction resource to map
    WriteLocker lock(m_lock);
    map<TaskID, AllocResource>::iterator it = IdToTempAllocResource.find(id);

    if(it != IdToTempAllocResource.end()) {
        it->second = alloc_resource;
        return true;
    }
    IdToTempAllocResource[id] = alloc_resource;
    return true;
}


void Machine::SetId(int id){
    m_id = id;
}

int Machine::GetId(){
    return m_id;
}

string Machine::GetIp() {
    return m_ip;
}

ClassAdPtr Machine::GetMachineAd(){
    return m_machine_ad;
}

/*
int Machine::GetTARMapSize() {
   return IdToTempAllocResource.size();
}

bool Machine::CheckAndClearTARMap(const TaskID& task_id) {
   WriteLocker lock(m_lock);
   map<TaskID, AllocResource>::iterator it = IdToTempAllocResource.find(task_id);
   if(it != IdToTempAllocResource.end()) {
        IdToTempAllocResource.erase(it);
        return true;
   }else {
	return false;
   }
}
*/

bool Machine::GetTemproryAllocResourceMap(map<TaskID, AllocResource>& TemproryAllocResourceMap) {
    ReadLocker lock(m_lock);    
    if (IdToTempAllocResource.size() == 0) {
	return false;
    } else{
        for(map<TaskID, AllocResource>::iterator it = IdToTempAllocResource.begin(); it != IdToTempAllocResource.end(); ++it) {
            //TemproryAllocResourceMap.insert(map<TaskID, AllocResource>::value_type(it->first, it->second));
	    TemproryAllocResourceMap[it->first] = it->second;
        }
        return true;
    }
}

