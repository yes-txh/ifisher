/********************************
 FileName: JobsManager/JobsManager.cpp
 Author:   ZhangZhang 
 Date:     2013-08-16
 Version:  0.1
 Description: JobsManager main
*********************************/
#include <iostream>

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/consoleappender.h>
#include <log4cplus/layout.h>
#include <gflags/gflags.h>

#include "include/proxy.h"
#include "common/clynn/rpc.h"
#include "JobsManager/group_pool.h"
#include "JobsManager/jobs_manager_service.h"
#include "JobsManager/dispatcher.h"

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

DECLARE_int32(jobs_manager_port);
DECLARE_string(resource_scheduler_endpoint);
DECLARE_string(groups_xml);

extern void* NewJobThread(void* unused);
extern void* JobProcessorThread(void* unused);
extern void* TaskCheckerThread(void* unused);
extern void* JobStateUpdaterThread(void* unused);

int main(int argc, char **argv){
    SharedObjectPtr<Appender> append(new FileAppender("../log/JobsManager.log"));
    append->setName(LOG4CPLUS_TEXT("append for JobsManager"));
    auto_ptr<Layout> layout(new PatternLayout(LOG4CPLUS_TEXT("%d{%m/%d/%y %H:%M:%S} %p %l:%m %n")));
    append->setLayout(layout);
    Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("JobsManager"));
    logger.addAppender(append);
    logger.setLogLevel(log4cplus::INFO_LOG_LEVEL);

    if(argc > 1) {
        if(!(google::ParseCommandLineFlags(&argc, &argv, true))) {
            LOG4CPLUS_ERROR(logger, "Error happens when parsing flags from command line");
            return EXIT_FAILURE;
        }
    } else {
        if(!(google::ReadFromFlagsFile("../conf/jobs_manager.conf", argv[0], true))) {
            LOG4CPLUS_ERROR(logger, "Error happens when parsing flags from file");
            return EXIT_FAILURE;
        }
    }
   
    LOG4CPLUS_INFO(logger, argv[0] << "daemon begin");

    if (!GroupPoolI::Instance()->InitGroups(FLAGS_groups_xml)) {
        LOG4CPLUS_ERROR(logger, "Failed to initialize groups by config file: " << FLAGS_groups_xml);
        return EXIT_FAILURE;
    }
    // test TODO
    GroupPoolI::Instance()->PrintAll();
    
    pthread_t new_job_thread_tid;
    pthread_create(&new_job_thread_tid, NULL, NewJobThread, NULL);

    pthread_t job_processor_tid;
    pthread_create(&job_processor_tid, NULL, JobProcessorThread, NULL);

    pthread_t task_checker_tid;
    pthread_create(&task_checker_tid, NULL, TaskCheckerThread, NULL);

    pthread_t job_updater_tid;
    pthread_create(&job_updater_tid, NULL, JobStateUpdaterThread, NULL);

    //image event
    Handler* image_event_handler = new Handler;
    image_event_handler->Start();
    EventDispatcherI::Instance()->Register(JMEventType::IMAGE_EVENT, image_event_handler);

    //app event
    Handler* app_event_handler = new Handler;
    app_event_handler->Start();
    EventDispatcherI::Instance()->Register(JMEventType::APP_EVENT, app_event_handler);

    // task action event 
    Handler* task_action_event_handler = new Handler;
    task_action_event_handler->Start();
    EventDispatcherI::Instance()->Register(JMEventType::TASK_ACTION_EVENT, task_action_event_handler);

    // task state event 
    Handler* task_state_event_handler = new Handler;
    task_state_event_handler->Start();
    EventDispatcherI::Instance()->Register(JMEventType::TASK_STATE_EVENT, task_state_event_handler);

    cout << "JobsManager is OK." << endl;

    RpcServer<JobsManagerService, JobsManagerProcessor>::Listen(FLAGS_jobs_manager_port);
    return EXIT_SUCCESS;
}
