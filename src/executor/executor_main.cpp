/********************************
 FileName: executor/executor.cpp
 Author:   WangMin
 Date:     2013-08-14
 Version:  0.1
 Description: executor main
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
#include "executor/system.h"
#include "executor/service.h"
#include "executor/dispatcher.h"
#include "executor/resource_manager.h"
#include "executor/hdfs_wrapper.h"
#include "executor/ip_pool.h"
#include "executor/executor_register.h"


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
DECLARE_int32(port);
DECLARE_string(collector_endpoint);
DECLARE_string(scheduler_endpoint);
/*DECLARE_string(interface);
DECLARE_string(img_dir);*/
DECLARE_string(log_path);
DECLARE_string(libvirt_dir);
DECLARE_string(xml_template);
DECLARE_string(lxc_dir);
DECLARE_string(lxc_template);
DECLARE_string(hdfs_host);

//extern void* TaskProcessor(void* unused);
extern void* VMProcessor(void* unused);
extern void* HeartbeatProcessor(void* unused);

// executor
int ExecutorEntity(int argc, char **argv) {
    // config file
    if (argc > 1)
        google::ParseCommandLineFlags(&argc, &argv, true);
    else
        google::ReadFromFlagsFile("../conf/executor.conf", argv[0], true);

    // get time
    /* char time_c[64] = {0};
    System::GetCurrentTime(time_c, sizeof(time_c)-1);
    string time_str = time_c;*/

    // initilize log log4cplus
    SharedObjectPtr<Appender> append(new FileAppender(FLAGS_log_path + "/executor.log"));
    append->setName(LOG4CPLUS_TEXT("append for executor"));
    auto_ptr<Layout> layout(new PatternLayout(LOG4CPLUS_TEXT("%d{%y/%m/%d %H:%M:%S} %p [%l]: %m %n")));
    append->setLayout(layout);
    Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("executor"));
    logger.addAppender(append);
    logger.setLogLevel(log4cplus::DEBUG_LOG_LEVEL);
    LOG4CPLUS_DEBUG(logger, "This is the FIRST debug message");
    LOG4CPLUS_INFO(logger, "This is the FIRST info message");
    LOG4CPLUS_ERROR(logger, "This is the FIRST error message");
  
    // TODO
    //check version
    ExecutorRegister* registerptr = new ExecutorRegister();
    if(registerptr->RegistMachine() == false) {
        delete registerptr;
	exit(1);
    }
    delete registerptr;

    // get current dir
    char cur_dir[100];
    if (getcwd(cur_dir, sizeof(cur_dir)-1) != NULL) {
        LOG4CPLUS_INFO(logger, "current dir:" << cur_dir);
    } else {
        LOG4CPLUS_ERROR(logger, "Failed to get current dir.");
    }
    string separator = "/";
    FLAGS_xml_template = cur_dir + separator + FLAGS_xml_template;
    FLAGS_lxc_template = cur_dir + separator + FLAGS_lxc_template;

    // init resource_manager
    if (!ResourceMgrI::Instance()->Init()) {
        LOG4CPLUS_ERROR(logger, "Failed to initialize resource manager.");
        return EXIT_FAILURE;
    }

    // init ip pool
    if (!IPPoolI::Instance()->Init()) {
        LOG4CPLUS_ERROR(logger, "Failed to initialize IPPool.");
        return EXIT_FAILURE;
    }
    // test TODO
    /*
    IPPoolI::Instance()->PrintAll();
    string ip =  IPPoolI::Instance()->GetAvailIp();
    IPPoolI::Instance()->PrintAll();
    if (!IPPoolI::Instance()->ReleaseIp(ip)) {
         LOG4CPLUS_ERROR(logger, "Failed to relase ip in IPPool.");
         return -1;
    } 
    IPPoolI::Instance()->PrintAll();
    */

    // work thread
    //pthread_t task_t;
    //pthread_create(&task_t, NULL, TaskProcessor, NULL);

    pthread_t vm_t;
    pthread_create(&vm_t, NULL, VMProcessor, NULL);

    // TODO
    pthread_t hb_t;
    pthread_create(&hb_t, NULL, HeartbeatProcessor, NULL);

    /* event dispatcher */
    //heartbeat event
    Handler* heartbeat_event_handler = new Handler;
    heartbeat_event_handler->Start();
    EventDispatcherI::Instance()->Register(EventType::HEARTBEAT_EVENT, heartbeat_event_handler);

    // task action event
    Handler* task_action_event_handler = new Handler;
    task_action_event_handler->Start();
    EventDispatcherI::Instance()->Register(EventType::TASK_ACTION_EVENT, task_action_event_handler);

    // task state event
    Handler* task_state_event_handler = new Handler;
    task_state_event_handler->Start();
    EventDispatcherI::Instance()->Register(EventType::TASK_STATE_EVENT, task_state_event_handler);

    //image event
    Handler* image_event_handler = new Handler;
    image_event_handler->Start();
    EventDispatcherI::Instance()->Register(EventType::IMAGE_EVENT, image_event_handler);

    //executor exit event
    Handler* executor_event_handler = new Handler;
    executor_event_handler->Start();
    EventDispatcherI::Instance()->Register(EventType::EXIT_EXECUTOR_EVENT, executor_event_handler);

    cout << "Executor is OK." << endl;

    // Listen for service 
    RpcServer<ExecutorService, ExecutorProcessor>::Listen(FLAGS_port);
    //int port = 9999; 
    //Rpc<ExecutorService, ExecutorProcessor>::Listen(port);

    return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
    // is root?
    if (geteuid() != 0) {
        fprintf(stderr, "Executor: must be run as root, or sudo run it.\n");
        return EXIT_FAILURE;
    }

    ExecutorEntity(argc, argv);
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
