/*******************************
 File name: submitter/image.cpp
 Author:   Tangxuehai
 Date:     2014-03-14
 Version:  0.1
 Description: image 
*********************************/

#include <string>
#include <vector>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include "submitter/hdfs_wrapper.h"
#include "submitter/image.h"
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

using log4cplus::Logger;
using std::string;
using std::vector;

using rapidxml::file;
using rapidxml::xml_document;
using rapidxml::xml_node;
using rapidxml::xml_attribute;

DECLARE_string(JobsManager_endpoint);
DECLARE_bool(first_create_image);

static Logger logger = Logger::getInstance("submitter");

bool Image::CreateImage(const string& xml_path){
    //read xml to init
    std::cout<< "init config start:"<<xml_path<<std::endl;
    m_xml_path = xml_path;
    if(false == Init()){
        LOG4CPLUS_ERROR(logger, "read xml init error");
	fprintf(stderr, "read xml init error\n");
        return false;
    }
    if(true == FLAGS_first_create_image){
        //check local image
        string image_local_path = m_local_path + "/" + m_name;
	std::cout<< "image_local_path:"<<image_local_path<<std::endl;
        if(access(image_local_path.c_str(), F_OK) == -1) {
            LOG4CPLUS_ERROR(logger, "image file no found");
	    fprintf(stderr, "image file no found\n");
            return false;
	}       
 
        //send image file to HDFS
        std::cout<< "upload image to hdfs start"<<std::endl;
        if(false == SendImagetoHDFS()){
            LOG4CPLUS_ERROR(logger, "send image file to hdfs  error");
            fprintf(stderr, "send image file to hdfs  error\n");
	    return false;
        }
        std::cout<< "upload image from hdfs finish"<<std::endl;
    }

    //update image on master
    std::cout<< "update image from hdfs start"<<std::endl;
    try {
           Proxy<JobsManagerClient> proxy = Rpc<JobsManagerClient, JobsManagerClient>::GetProxy(FLAGS_JobsManager_endpoint);
           if(false == proxy().UpdateImage(m_user, m_name, m_size, m_is_update_all)){
               LOG4CPLUS_ERROR(logger, "update image faile, master error");
	       fprintf(stderr, "update image faile, rpc error");  
               return false;
           }
       }  catch (TException &tx) {
          //std::cout<< "rpc error"<<std::endl;
          LOG4CPLUS_ERROR(logger, "update image faile,rpc error");
	  fprintf(stderr, "update image faile,rpc error");
          return false;
       }
    LOG4CPLUS_INFO(logger, "update image finished");
    std::cout<< "update image from hdfs finish"<<std::endl;
    return true;
}


bool Image::Init(){
    xml_document<> doc;
    try {
        file<> fdoc(m_xml_path.c_str());
        doc.parse<0>(fdoc.data());
    } catch (rapidxml::parse_error& ex) {
        fprintf(stderr, "synex error in %s.\n", ex.what());
        return false;
    } catch (std::runtime_error& ex) {
        fprintf(stderr, "task xml error:%s.\n", ex.what());
        return false;
    }
    xml_node<> *root = doc.first_node("submitter_img");

    if(!root) {
        fprintf(stderr, "no submitter_img specified.");
        return false;
    }
    xml_node<> *node1;

    node1 = root->first_node("user");
    if(!node1) {
        fprintf(stderr, "no user interval specified.");
        return false;
    }
    m_user = node1->value();

    node1 = node1->next_sibling("name");
    if(!node1) {
        fprintf(stderr, "no name interval specified.");
        return false;
    }
    m_name = node1->value();

    node1 = node1->next_sibling("size");
    if(!node1) {
        fprintf(stderr, "no size interval specified.");
        return false;
    }
    string size = node1->value();
    m_size = atoi(size.c_str());

    node1 = node1->next_sibling("is_update_all");
    if(!node1) {
        fprintf(stderr, "no is_update_all interval specified.");
        return false;
    }
    string xml_str = node1->value();
    if(xml_str == "true") {
        m_is_update_all = true;
    } else if(xml_str == "false") {
	m_is_update_all = false;
    } else {
        fprintf(stderr, "is_update_all is error.");
        return false;
}

    if(true == FLAGS_first_create_image){
        node1 = node1->next_sibling("local_path");
        if(!node1) {
            fprintf(stderr, "no local_path interval specified.");
            return false;
        }
        m_local_path= node1->value();
    }
    return true;
}


bool Image::SendImagetoHDFS(){
    const string image_hdfs_path = "/lynn/" + m_user + "/img/" + m_name;
    const string image_local_path = m_local_path + "/"+ m_name;
 
    //init hdfs
    HDFSMgrI::Instance()->Init();

    //copy local image to hdfs
    //std::cout<< "image:"<<image_local_path<<std::endl;
    if(false == HDFSMgrI::Instance()->CopyFromLocalFile(image_local_path, image_hdfs_path)){
        LOG4CPLUS_ERROR(logger, "copy local imge to hdfs error");
	fprintf(stderr, "copy local image to hdfs error\n");
        return false;
    }

    //read image on hdfs to make sure send image to hdfs is ok

    return true;
    }
/*
bool Image::CreateImage(const string& xml_path){
    //read xml to init
    std::cout<< "init config start"<<xml_path<<std::endl;

    if(false == Init(xml_path)){
	LOG4CPLUS_ERROR(logger, "read xml init error");
        return false;
    }
    std::cout<< "init config"<<std::endl;

    if(true == FLAGS_first_create_image){
        //send image file to HDFS
        std::cout<< "upload image start"<<std::endl;
        if(false == SendImagetoHDFS()){
            LOG4CPLUS_ERROR(logger, "send image file to hdfs  error");
            return false;
        }
        std::cout<< "upload image finish"<<std::endl;
    }

    //update image on master
    try {
           Proxy<MasterClient> proxy = Rpc<MasterClient, MasterClient>::GetProxy(FLAGS_scheduler_endpoint);
           if(false == proxy().UpdateImage(m_name, m_user, m_size)){
               LOG4CPLUS_ERROR(logger, "update image faile, master error");
               return false;
           }
       }  catch (TException &tx) {
          //std::cout<< "rpc error"<<std::endl;
          LOG4CPLUS_ERROR(logger, "update image faile,rpc error");
          return false;
       }
    LOG4CPLUS_INFO(logger, "update image finished");
    std::cout<< "update image finish"<<std::endl;
    return true;
}
*/
/*
bool Image::DeleteImage(const int32_t image_id){
    //delete image config
    if(false == ImageConfigPtrI::Instance()->DeleteImageConfig(image_id)){
        LOG4CPLUS_ERROR(logger, "delete image faile");
        return false;
    }
    //delete image file on hdfs
    string image_config_ad;
    if(false == ImageConfigPtrI::Instance()->QueryImageConfigAd(image_config_ad, image_id)){
        LOG4CPLUS_ERROR(logger, "query image config ad faile");
        return false;
    }
    
    string image_name;
    string image_user;
    //get value from classad to image_config
    ClassAdParser parser;
    ClassAd* ad_ptr = parser.ParseClassAd(image_config_ad);

    //parse image_name
    if (!ad_ptr->EvaluateAttrString(ATTR_IMAGE_NAME, image_name)) {
        LOG4CPLUS_ERROR(logger, "Fails to init image, because parse " << ATTR_IMAGE_NAME << " error.");
        return false;
    }

    //parse image_user
    if (!ad_ptr->EvaluateAttrString(ATTR_IMAGE_USER, image_user)) {
        LOG4CPLUS_ERROR(logger, "Fails to init image, because parse " << ATTR_INSTALL_DIR << " error.");
        return false;
    }

    if(image_name.empty() || image_user.empty()){
        LOG4CPLUS_ERROR(logger, "Fails to create image, because parse classad is null");
        return false;
    }

    if(false == ImageFilePtrI::Instance()->DeleteImageFile(image_name, image_user)){
        LOG4CPLUS_ERROR(logger, "delete image file faile");
        return false;
    }
    return true;
}

bool Image::QueryImage(const int32_t image_id){
    if(false == ImageConfigPtrI::Instance()->QueryImageConfig(image_id)){
        LOG4CPLUS_ERROR(logger, "query image  faile");
        return false;
    }
    return true;
}

bool Image::QueryImageList(){
    if(false == ImageConfigPtrI::Instance()->QueryImageConfigList()){
        LOG4CPLUS_ERROR(logger, "query image list faile");
        return false;
    }
    return true;
}
*/

