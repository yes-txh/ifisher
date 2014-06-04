/********************************
 FileName: submitter/submitter.cpp
 Author:   Tangxuehai
 Date:     2013-11-19
 Version:  0.1
 Description: submitter main
*********************************/

#include <stdlib.h>
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

#include "submitter/cmd_parse.h"


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

// gflag, config for submitter
DECLARE_string(object);//app or job...
DECLARE_string(operation);// create or delete...
/*
DECLARE_int32(app_id);
DECLARE_int32(job_id);
DECLARE_int32(user_id);
DECLARE_int32(image_id);
*/
int main(int argc, char **argv) {
    //is root ?
    if (geteuid() != 0) {
        fprintf(stderr, "submittor: must be run as root, or sudo run it.\n");
        return EXIT_FAILURE;
    }

    // config file
    if (argc > 1)
        google::ParseCommandLineFlags(&argc, &argv, true);
    else
        google::ReadFromFlagsFile("../conf/submitter.conf", argv[0], true);

    // initilize log log4cplus
    SharedObjectPtr<Appender> append(new FileAppender("../log/submitter.log"));
    append->setName(LOG4CPLUS_TEXT("append for submitter"));
    auto_ptr<Layout> layout(new PatternLayout(LOG4CPLUS_TEXT("%d{%y/%m/%d %H:%M:%S} %p [%l]: %m %n")));
    append->setLayout(layout);
    Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("submitter"));
    logger.addAppender(append);
    logger.setLogLevel(log4cplus::DEBUG_LOG_LEVEL);
    LOG4CPLUS_DEBUG(logger, "This is the FIRST debug message");
    LOG4CPLUS_INFO(logger, "This is the FIRST info message");
    LOG4CPLUS_ERROR(logger, "This is the FIRST error message");


    //parse run_mode
   if(FLAGS_object == "User"){
        CmdParsePtrI::Instance()->UserCmdParse();
        LOG4CPLUS_INFO(logger, "User cmd parse begin");
    }else if(FLAGS_object == "App"){
       	CmdParsePtrI::Instance()->AppCmdParse();
	LOG4CPLUS_INFO(logger, "App cmd parse begin");
    }else if(FLAGS_object == "Job"){
	CmdParsePtrI::Instance()->JobCmdParse();
        LOG4CPLUS_INFO(logger, "Job cmd parse begin");
    }else if(FLAGS_object == "Image"){
	CmdParsePtrI::Instance()->ImageCmdParse();
        LOG4CPLUS_INFO(logger, "Image cmd parse begin");	
    }else if(FLAGS_object == "default"){
	CmdParsePtrI::Instance()->DefaultCmdParse();
        LOG4CPLUS_INFO(logger, "Default cmd parse begin");
    }else{
	CmdParsePtrI::Instance()->UsageCmdParse();
        LOG4CPLUS_ERROR(logger, "cmd parse error, unknown run object");
	return -1;
    }
    return 0; 
}
