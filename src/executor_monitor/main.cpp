/********************************
 FileName: executor_monitor/main.cpp
 Author:   Tang Xuehai
 Date:     2014-05-24
 Version:  0.1
 Description: executor_monitor main
*********************************/

#include <iostream>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/consoleappender.h>
#include <log4cplus/layout.h>
#include <gflags/gflags.h>

#include "common/clynn/rpc.h"
#include "executor_monitor/hdfs_wrapper.h"
#include "executor_monitor/checker.h"


using std::string;
using std::cout;
using std::endl;
using std::auto_ptr;

using log4cplus::Logger;
using log4cplus::ConsoleAppender;
using log4cplus::FileAppender;
using log4cplus::Appender;
using log4cplus::Layout;
using log4cplus::PatternLayout;
using log4cplus::helpers::SharedObjectPtr;

// gflag, config for executor
DECLARE_string(log_path);
DECLARE_string(hdfs_host);

//extern void* TaskProcessor(void* unused);
extern void* ExecutorMonitorProcessor(void* unused);

// executor
int ExecutorMonitorEntity(int argc, char **argv) {
    // config file
    if (argc > 1)
        google::ParseCommandLineFlags(&argc, &argv, true);
    else
        google::ReadFromFlagsFile("../conf/executor.conf", argv[0], true);

    // initilize log log4cplus
    SharedObjectPtr<Appender> append(new FileAppender(FLAGS_log_path + "/executor_monitor.log"));
    append->setName(LOG4CPLUS_TEXT("append for executor monitor"));
    auto_ptr<Layout> layout(new PatternLayout(LOG4CPLUS_TEXT("%d{%y/%m/%d %H:%M:%S} %p [%l]: %m %n")));
    append->setLayout(layout);
    Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("executor_monitor"));
    logger.addAppender(append);
    logger.setLogLevel(log4cplus::DEBUG_LOG_LEVEL);
    LOG4CPLUS_DEBUG(logger, "This is the FIRST debug message");
    LOG4CPLUS_INFO(logger, "This is the FIRST info message");
    LOG4CPLUS_ERROR(logger, "This is the FIRST error message");
  

    pthread_t em_t;
    pthread_create(&em_t, NULL, ExecutorMonitorProcessor, NULL);

    cout << "Executor Monitor is OK." << endl;

    while(true) {

    }

    return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
    // is root?
    if (geteuid() != 0) {
        fprintf(stderr, "Executor: must be run as root, or sudo run it.\n");
        return EXIT_FAILURE;
    }

    ExecutorMonitorEntity(argc, argv);
    // monitor ExecutorEntity
    /* while(true) {
        int32_t status;
        int32_t pid = fork();
        if (pid != 0) {
            // parent process, start executorEntity when ExecutorEntity fail
            if (waitpid(pid, &status, 0) > 0) {
                continue;
            }
        } else {
            // child process
            ExecutorEntity(argc, argv);
        }
    }*/
    return EXIT_SUCCESS;
}
