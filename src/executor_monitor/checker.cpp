/********************************
 FileName: executor_monitor/checker.cpp
 Author:   Tangxuehai
 Date:     2014-05-24
 Version:  0.1
 Description: checker
*********************************/
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <gflags/gflags.h>
#include <iostream>
#include <fstream>
#include <string>

#include "include/proxy.h"
#include "common/clynn/rpc.h"
#include "executor_monitor/checker.h"
#include "executor_monitor/hdfs_wrapper.h"

DECLARE_string(resource_scheduler_endpoint);
DECLARE_string(lynn_version);
DECLARE_string(executor_hdfs_path);


using log4cplus::Logger;
using namespace std;

static Logger logger = Logger::getInstance("executor_monitor");

bool Checker::CheckVersion() {
    // check executor version
    string new_lynn_version;
    //update executor version in executor.conf
    char cur_dir[100];
    if (getcwd(cur_dir, sizeof(cur_dir)-1) == NULL) {
         LOG4CPLUS_ERROR(logger, "Failed to get current dir.");
         return false;
    }
    string executor_dir(cur_dir);

    try {
        Proxy<ResourceSchedulerClient> proxy = RpcClient<ResourceSchedulerClient>::GetProxy(FLAGS_resource_scheduler_endpoint);
        proxy().GetCurVersion(new_lynn_version);
	if(new_lynn_version !=FLAGS_lynn_version) {
             LOG4CPLUS_ERROR(logger, "executor version is error, old_version:" << FLAGS_lynn_version << "  new_version:" << new_lynn_version);
	     FLAGS_lynn_version = new_lynn_version;     	
	     //update executor from hdfs
             if(UpdateExecutorFromHdfs() == false) {
                return false;
             }

	     string executor_conf_file = executor_dir + "/../conf/executor.conf";
	     
             string cmd_del = "sed -i '/lynn_version/d' " + executor_conf_file;
	     std::cout << "cmd del:" << cmd_del << std::endl;
             system(cmd_del.c_str());
             string cmd_write = "echo  --lynn_version=" + FLAGS_lynn_version  + " >> " + executor_conf_file;	
	     std::cout << "cmd write:" << cmd_write << std::endl;
             system(cmd_write.c_str());
        } else {
		string executor_file = executor_dir + "/executor";
    		ifstream in_file(executor_file.c_str(), ios::in);
    		//open file error ?
    		if(!in_file){
		    //update executor from hdfs
	            if(UpdateExecutorFromHdfs() == false) {
                       return false;
                    }
    		}
	}
    } catch (TException &tx) {
        LOG4CPLUS_ERROR(logger, "rpc error: check version is failed " << tx.what());
        return false;
    }
    return true;
}

bool Checker::UpdateExecutorFromHdfs() {
    // get current dir
    char cur_dir[100];
    if (getcwd(cur_dir, sizeof(cur_dir)-1) == NULL) {
        LOG4CPLUS_ERROR(logger, "Failed to get current dir.");
	return false;
    }
    string executor_dir(cur_dir);
    std::cout << "executor local path:" << executor_dir << "\nexecutor hdfs path:" << FLAGS_executor_hdfs_path <<std::endl;
    //rm old executor
    string cmd_rm = "rm -rf " + executor_dir + "/executor";
    system(cmd_rm.c_str());

    //open hdfs	
    HDFSMgrI::Instance()->OpenConnect();
    if(false == HDFSMgrI::Instance()->CopyToLocalFile(FLAGS_executor_hdfs_path, executor_dir)){
        LOG4CPLUS_ERROR(logger, "copy hdfs imge to local error");
        //fprintf(stderr, "copy local image to hdfs error");
        return false;
    }
    //close hdfs
    HDFSMgrI::Instance()->CloseConnect();

    string executor_file = executor_dir + "/executor";
    ifstream in_file(executor_file.c_str(), ios::in);
    //open file error ?
    if(!in_file){
        LOG4CPLUS_ERROR(logger, "executor file is no exist.");
        return false;
    }

    //add write mode to executor
    string cmd = "chmod +x  " + executor_dir + "/executor";
    std::cout<<"chmod cmd:"<<cmd<<std::endl;
    system(cmd.c_str());
    std::cout<< "executor update from hdfs finished"<<std::endl;
    return true;
}

bool Checker::IsProcess(const string& process_name) {
    FILE* fp;
    char buf[256];
    string cmd;
    int count;
    string name = process_name;
    cmd = "ps -C " + name +" |wc -l";
    if((fp=popen(cmd.c_str(), "r")) == NULL) {
	LOG4CPLUS_ERROR(logger, "popen error");
	return false;
    }

    if((fgets(buf, 128, fp))!= NULL ) {
	count = atoi(buf);
	if(count-1 == 1) {
	    return true;
	} else {
	    LOG4CPLUS_ERROR(logger, "process not find");
	    return false;
	}
    } else {
	LOG4CPLUS_ERROR(logger, "get process num from file  error");
	return false;
    }
}
