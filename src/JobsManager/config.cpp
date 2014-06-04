/********************************
 FileName: JobsManager/config.cpp
 Author:   WangMin
 Date:     2014-03-29
 Version:  0.1
 Description: config for JobsManager
*********************************/

#include "gflags/gflags.h"

DEFINE_int32(jobs_manager_port, 9997, "JobsManager rpc port");
DEFINE_string(resource_scheduler_endpoint, "127.0.0.1:9998", "resource_scheduler endpoint");
DEFINE_string(groups_xml, "../conf/groups.xml", "groups config file with xml format");
DEFINE_int32(max_restart_times, 3, "max times of restarting task");
