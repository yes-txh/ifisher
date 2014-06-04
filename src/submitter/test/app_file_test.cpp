/********************************
 FileName: submitter/app_file_test.cpp
 Author:   Tangxuehai
 Date:     2013-11-19
 Version:  0.1
 Description: app_file_test main
*********************************/

#include <iostream>
#include <sys/stat.h>
#include <sys/wait.h>

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/consoleappender.h>
#include <log4cplus/layout.h>
#include <gflags/gflags.h>

#include "submitter/app_file.h"

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


int main(int argc, char **argv) {

    // initilize log log4cplus
    SharedObjectPtr<Appender> append(new FileAppender("submitter.log"));
    append->setName(LOG4CPLUS_TEXT("append for submitter"));
    auto_ptr<Layout> layout(new PatternLayout(LOG4CPLUS_TEXT("%d{%y/%m/%d %H:%M:%S} %p [%l]: %m %n")));
    append->setLayout(layout);
    Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("submitter"));
    logger.addAppender(append);
    logger.setLogLevel(log4cplus::DEBUG_LOG_LEVEL);
    LOG4CPLUS_DEBUG(logger, "This is the FIRST debug message");
    LOG4CPLUS_INFO(logger, "This is the FIRST info message");
    LOG4CPLUS_ERROR(logger, "This is the FIRST error message");

    string app_file_name = "a.txt";
    string app_file_source = "/user/root/test";
    string app_file_locate = "/home/yesw/";

    if(false == AppFilePtrI::Instance()->CreateAppFile(app_file_name, app_file_locate, app_file_source)){
        //LOG4CPLUS_ERROR(logger, "cmd parse error, unknown run mode");
	cout<< "create app file to hdfs error"<< endl;
	return -1;
    }
    return 0; 
}  
