/********************************
 FileName: executor_monitor/restart_executor.cpp
 Author:   Tangxuehai
 Date:     2014-05-24
 Version:  0.1
 Description: restart executor 
*********************************/
#include <stdlib.h>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <gflags/gflags.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>


#include "executor_monitor/restart_executor.h"
#include "executor_monitor/checker.h"

using log4cplus::Logger;

static Logger logger = Logger::getInstance("executor_monitor");

bool RestartExecutor::CheckVersionRestartExecutor() {
    //check version
    Checker* checkerptr = new Checker();
    bool rt = checkerptr->CheckVersion();
    delete checkerptr;

    if(rt != true) {
	LOG4CPLUS_ERROR(logger, "Failed to check executor version.");
	return false;
    }
    //restart executor
    // get current dir
    char cur_dir[100];
    if (getcwd(cur_dir, sizeof(cur_dir)-1) == NULL) {
        LOG4CPLUS_ERROR(logger, "Failed to get current dir.");
    }

    string dir_str(cur_dir);
    string cmd;
    cmd = "nohup " + dir_str + "/executor  >/dev/null 2>&1 &";
    std::cout << "cmd:" << cmd <<std::endl;
    system(cmd.c_str());
    return true;
}

