/*******************************
 File name: submitter/cmd_parse.cpp
 Author:   Tangxuehai
 Date:     2013-11-19
 Version:  0.1
 Description: cmd parse
*********************************/

#include "submitter/cmd_parse.h"
#include "submitter/image.h"
#include "submitter/app.h"
#include "submitter/job.h"

#include <iostream>
#include <gflags/gflags.h>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

using log4cplus::Logger;
using std::cout;
using std::endl;

static Logger logger = Logger::getInstance("submitter");

DECLARE_string(operation);// create or delete...
DECLARE_string(xml_path); //classad string 

DECLARE_int32(job_id);

void CmdParse::UserCmdParse(){
/*    if(FLAGS_operation == "create"){
	UserPtrI::Instance()->CreateUser(FLAGS_xml_path);
    }else if(FLAGS_operation == "delete"){
	UserPtrI::Instance()->DeleteUser(FLAGS_user_id);
    }else if(FLAGS_operation == "update"){
//	UserPtrI::Instance()->UpdateUser(FLAGS_str_ad);
    }else if(FLAGS_operation == "query"){
	UserPtrI::Instance()->QueryUser(FLAGS_user_id);
    }else if(FLAGS_operation == "query_list"){
	UserPtrI::Instance()->QueryUserList();
    }else{
	LOG4CPLUS_ERROR(logger, "user cmd parse error, unknown run type");
	cout<< "User run type usage:"<<endl;
	cout<< "create: create user on system"<<endl;
	cout<< "delete: delete user from system"<<endl;
	cout<< "update: update user on system"<<endl;
	cout<< "query: query user on system"<<endl;
	cout<< "query_list: query user list on system"<<endl;
    }
*/    return;
}


void CmdParse::AppCmdParse(){
    if(FLAGS_operation == "create"){
	AppPtrI::Instance()->CreateApp(FLAGS_xml_path);
    }
    /*else if(FLAGS_operation == "delete"){
	AppPtrI::Instance()->DeleteApp(FLAGS_app_id);
    }else if(FLAGS_operation == "update"){
//	AppPtrI::Instance()->UpdateApp(FLAGS_str_ad);
    }else if(FLAGS_operation == "query"){
	AppPtrI::Instance()->QueryApp(FLAGS_app_id);
    }else if(FLAGS_operation == "query_list"){
	AppPtrI::Instance()->QueryAppList();
    }*/
    else{
	LOG4CPLUS_ERROR(logger, "app cmd parse error, unknown run type");
	cout<< "App run type usage:"<<endl;
	cout<< "create: create app on system"<<endl;
	cout<< "delete: delete app from system"<<endl;
	cout<< "query: query app on system"<<endl;
	cout<< "query_list: query app list on system"<<endl;
    }
    return;
}

void CmdParse::ImageCmdParse(){
    if(FLAGS_operation == "create"){
        cout<< "xml_path:"<<FLAGS_xml_path<<endl;
        ImagePtrI::Instance()->CreateImage(FLAGS_xml_path);
    }
     /*else if(FLAGS_operation == "delete"){
        ImagePtrI::Instance()->DeleteImage(FLAGS_image_id);
    }else if(FLAGS_operation == "query"){
        ImagePtrI::Instance()->QueryImage(FLAGS_image_id);
    }else if(FLAGS_operation == "query_list"){
        ImagePtrI::Instance()->QueryImageList();
    }*/
    else{
        LOG4CPLUS_ERROR(logger, "image cmd parse error, unknown run type");
        cout<< "Image run type usage:"<<endl;
        cout<< "create: create image on system, write image file to HDFS"<<endl;
        cout<< "delete: delete image from system"<<endl;
        cout<< "query: query image on system"<<endl;
        cout<< "query_list: query image list on system"<<endl;
    }
    return;
}


void CmdParse::JobCmdParse(){
    if(FLAGS_operation == "create"){
        cout<< "xml_path:"<<FLAGS_xml_path<<endl;
        JobPtrI::Instance()->CreateJob(FLAGS_xml_path);
    }
    else if(FLAGS_operation == "query"){
        JobPtrI::Instance()->QueryJob(FLAGS_job_id);
    }/*else if(FLAGS_operation == "query_list"){
        ImagePtrI::Instance()->QueryImageList();
    }*/
    else{
        LOG4CPLUS_ERROR(logger, "image cmd parse error, unknown run type");
        cout<< "Image run type usage:"<<endl;
        cout<< "create: create image on system, write image file to HDFS"<<endl;
        cout<< "delete: delete image from system"<<endl;
        cout<< "query: query image on system"<<endl;
        cout<< "query_list: query image list on system"<<endl;
    }
    return;

    cout<<"Hello World !"<<endl;
    return;
}

void CmdParse::DefaultCmdParse(){
    return;
}

void CmdParse::UsageCmdParse(){
     cout<< "Run mode usage:"<<endl;
     cout<< "Image: Image manager mode"<<endl;
     cout<< "App: App manager mode"<<endl;
     cout<< "Job: job manager mode"<<endl;
     cout<< "Default: Default manager mode"<<endl;
     return;
}
