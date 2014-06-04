/*******************************
 File name: submitter/app.cpp
 Author:   Tangxuehai
 Date:     2014-04-23
 Version:  0.1
 Description: app manager for submitter
*********************************/

#include <string>
#include <vector>
#include <stdio.h>
#include <iostream>
#include "submitter/hdfs_wrapper.h"
#include "submitter/app.h"
#include "include/classad_attr.h"
#include "include/proxy.h"

#include "common/rapidxml/rapidxml.hpp"
#include "common/rapidxml/rapidxml_utils.hpp"
#include "common/clynn/rpc.h"

#include <classad/classad.h>
#include <classad/classad_distribution.h>

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

#include <gflags/gflags.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

using log4cplus::Logger;
using std::string;
using std::vector;

using rapidxml::file;
using rapidxml::xml_document;
using rapidxml::xml_node;
using rapidxml::xml_attribute;

DECLARE_string(JobsManager_endpoint);

static Logger logger = Logger::getInstance("submitter");

bool App::CreateApp(const string& xml_path){
    //read xml to init
    std::cout<< "init config start:"<<xml_path<<std::endl;
    m_xml_path = xml_path;
    if(false == Init()){
        //LOG4CPLUS_ERROR(logger, "read xml init error");
	fprintf(stderr, "read xml init error\n");
        return false;
    }
    //check local app
    string app_local_path = m_local_path;
    std::cout<< "app_local_path:"<<app_local_path<<std::endl;
    if(access(app_local_path.c_str(), F_OK) == -1) {
        //LOG4CPLUS_ERROR(logger, "app file no found");
	fprintf(stderr, "app file no found\n");
        return false;
    }       
 
    //send app file to HDFS
    std::cout<< "upload app to hdfs start"<<std::endl;
    if(false == SendApptoHDFS()){
       //LOG4CPLUS_ERROR(logger, "send app file to hdfs  error");
       fprintf(stderr, "send app file to hdfs  error\n");
       return false;
    }
    std::cout<< "upload app to hdfs finish"<<std::endl;
    

    //update app on master
    std::cout<< "update app from hdfs start"<<std::endl;
    try {
           Proxy<JobsManagerClient> proxy = Rpc<JobsManagerClient, JobsManagerClient>::GetProxy(FLAGS_JobsManager_endpoint);
           if(false == proxy().CreateApp(m_user, m_name)){
               //LOG4CPLUS_ERROR(logger, "update app faile, master error");
	       fprintf(stderr, "update app faile, rpc error");  
               return false;
           }
       }  catch (TException &tx) {
          //std::cout<< "rpc error"<<std::endl;
          //LOG4CPLUS_ERROR(logger, "update app faile,rpc error");
	  fprintf(stderr, "update app faile,rpc error");
          return false;
       }
    LOG4CPLUS_INFO(logger, "update app finished");
    std::cout<< "update app from hdfs finish"<<std::endl;
    return true;
}


bool App::Init(){
    xml_document<> doc;
    try {
        file<> fdoc(m_xml_path.c_str());
        doc.parse<0>(fdoc.data());
    } catch (rapidxml::parse_error& ex) {
        fprintf(stderr, "synex error in %s.\n", ex.what());
        return false;
    } catch (std::runtime_error& ex) {
        fprintf(stderr, "app xml error:%s.\n", ex.what());
        return false;
    }
    xml_node<> *root = doc.first_node("submitter_app");

    if(!root) {
        fprintf(stderr, "no submitter_app specified.\n");
        return false;
    }
    xml_node<> *node1;

    node1 = root->first_node("user");
    if(!node1) {
        fprintf(stderr, "no user interval specified.\n");
        return false;
    }
    m_user = node1->value();

    node1 = node1->next_sibling("name");
    if(!node1) {
        fprintf(stderr, "no name interval specified.\n");
        return false;
    }
    m_name = node1->value();

    node1 = node1->next_sibling("app_local_path");
    if(!node1) {
        fprintf(stderr, "no app_local_path interval specified.\n");
        return false;
    }
    m_local_path= node1->value();
    return true;
}


bool App::SendApptoHDFS(){
    //init hdfs
    HDFSMgrI::Instance()->Init();

    //copy local app to hdfs
    int pos = 0;
    m_local_path.rfind("/", pos);
    vector<string> m_list;
    boost::split(m_list, m_local_path, boost::is_any_of( "/" ), boost::token_compress_on);

    string m_exe = m_list.back();
    const string app_hdfs_path = "/lynn/" + m_user + "/app/" + m_name + "/" + m_exe;
    const string app_local_path = m_local_path;
    if(false == HDFSMgrI::Instance()->CopyFromLocalFile(app_local_path, app_hdfs_path)){
        //LOG4CPLUS_ERROR(logger, "copy local imge to hdfs error");
	fprintf(stderr, "copy local app to hdfs error\n");
        return false;
    }

    //read app on hdfs to make sure send image to hdfs is ok

    return true;
    }
/*
bool App::CreateImage(const string& xml_path){
    //read xml to init
    std::cout<< "init config start"<<xml_path<<std::endl;

    if(false == Init(xml_path)){
	LOG4CPLUS_ERROR(logger, "read xml init error");
        return false;
    }
    std::cout<< "init config"<<std::endl;

    if(true == FLAGS_first_create_app){
        //send app file to HDFS
        std::cout<< "upload app start"<<std::endl;
        if(false == SendApptoHDFS()){
            LOG4CPLUS_ERROR(logger, "send app file to hdfs  error");
            return false;
        }
        std::cout<< "upload app finish"<<std::endl;
    }

    //update app on master
    try {
           Proxy<MasterClient> proxy = Rpc<MasterClient, MasterClient>::GetProxy(FLAGS_scheduler_endpoint);
           if(false == proxy().UpdateApp(m_name, m_user, m_size)){
               LOG4CPLUS_ERROR(logger, "update app faile, master error");
               return false;
           }
       }  catch (TException &tx) {
          //std::cout<< "rpc error"<<std::endl;
          LOG4CPLUS_ERROR(logger, "update app faile,rpc error");
          return false;
       }
    LOG4CPLUS_INFO(logger, "update app finished");
    std::cout<< "update app finish"<<std::endl;
    return true;
}
*/
/*
bool App::DeleteImage(const int32_t app_id){
    //delete app config
    if(false == AppConfigPtrI::Instance()->DeleteImageConfig(app_id)){
        LOG4CPLUS_ERROR(logger, "delete app faile");
        return false;
    }
    //delete app file on hdfs
    string app_config_ad;
    if(false == AppConfigPtrI::Instance()->QueryImageConfigAd(app_config_ad, image_id)){
        LOG4CPLUS_ERROR(logger, "query app config ad faile");
        return false;
    }
    
    string app_name;
    string app_user;
    //get value from classad to app_config
    ClassAdParser parser;
    ClassAd* ad_ptr = parser.ParseClassAd(app_config_ad);

    //parse app_name
    if (!ad_ptr->EvaluateAttrString(ATTR_IMAGE_NAME, app_name)) {
        LOG4CPLUS_ERROR(logger, "Fails to init app, because parse " << ATTR_IMAGE_NAME << " error.");
        return false;
    }

    //parse app_user
    if (!ad_ptr->EvaluateAttrString(ATTR_IMAGE_USER, app_user)) {
        LOG4CPLUS_ERROR(logger, "Fails to init app, because parse " << ATTR_INSTALL_DIR << " error.");
        return false;
    }

    if(app_name.empty() || image_user.empty()){
        LOG4CPLUS_ERROR(logger, "Fails to create app, because parse classad is null");
        return false;
    }

    if(false == AppFilePtrI::Instance()->DeleteImageFile(app_name, image_user)){
        LOG4CPLUS_ERROR(logger, "delete app file faile");
        return false;
    }
    return true;
}

bool App::QueryImage(const int32_t app_id){
    if(false == AppConfigPtrI::Instance()->QueryImageConfig(app_id)){
        LOG4CPLUS_ERROR(logger, "query app  faile");
        return false;
    }
    return true;
}

bool App::QueryImageList(){
    if(false == AppConfigPtrI::Instance()->QueryImageConfigList()){
        LOG4CPLUS_ERROR(logger, "query app list faile");
        return false;
    }
    return true;
}
*/

