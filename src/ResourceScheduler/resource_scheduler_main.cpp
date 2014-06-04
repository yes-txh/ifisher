/********************************
 FileName: ResourceScheduler/resource_scheduler.cpp
 Author:   ZhangZhang 
 Date:     2013-08-16
 Version:  0.1
 Description: ResourceScheduler main
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
#include "ResourceScheduler/resource_scheduler_service.h"
#include "ResourceScheduler/dispatcher.h"


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

DEFINE_int32(resource_scheduler_port, 9998, "ResourceScheduler rpc port");

extern void* MachineMonitorProcessor(void* unused);

int main(int argc, char **argv){
    SharedObjectPtr<Appender> append(new FileAppender("../log/ResourceScheduler.log"));
    // SharedObjectPtr<Appender> append(new ConsoleAppender());
    append->setName(LOG4CPLUS_TEXT("append for ResourceScheduler"));
    auto_ptr<Layout> layout(new PatternLayout(LOG4CPLUS_TEXT("%d{%m/%d/%y %H:%M:%S} %p %l:%m %n")));
    append->setLayout(layout);
    Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("ResourceScheduler"));
    logger.addAppender(append);
    logger.setLogLevel(log4cplus::INFO_LOG_LEVEL);

    if(argc > 1) {
        if(!(google::ParseCommandLineFlags(&argc, &argv, true))) {
            LOG4CPLUS_ERROR(logger, "Error happens when parsing flags from command line");
            return EXIT_FAILURE;
        }
    } else {
        if(!(google::ReadFromFlagsFile("../conf/resource_scheduler.conf", argv[0], true))) {  
            LOG4CPLUS_ERROR(logger, "Error happens when parsing flags from file");
            return EXIT_FAILURE;
        }
    }
    
    LOG4CPLUS_INFO(logger, argv[0] << " daemon begin");

    pthread_t machine_monitor_t;
    pthread_create(&machine_monitor_t, NULL, MachineMonitorProcessor, NULL);

    /* event dispatcher */
    //machine update  event
    Handler* machine_update_event_handler = new Handler;
    machine_update_event_handler->Start();
    EventDispatcherI::Instance()->Register(RSEventType::MACHINE_UPDATE_EVENT, machine_update_event_handler);

    //machine delete  event
    Handler* machine_delete_event_handler = new Handler;
    machine_delete_event_handler->Start();
    EventDispatcherI::Instance()->Register(RSEventType::MACHINE_DELETE_EVENT, machine_delete_event_handler);


    RpcServer<ResourceSchedulerService, ResourceSchedulerProcessor>::Listen(FLAGS_resource_scheduler_port);
    return EXIT_SUCCESS;
}
