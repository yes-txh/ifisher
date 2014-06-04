/*******************************
 File name: submitter/image_config.cpp
 Author:   Tangxuehai
 Date:     2013-11-18
 Version:  0.1
 Description: image config 
*********************************/

#include <strstream>
#include <vector>
#include "submitter/image_config.h"
#include "include/classad_attr.h"
#include "include/proxy.h"
#include "common/clynn/rpc.h"
#include "common/rapidxml/rapidxml.hpp"
#include "common/rapidxml/rapidxml_utils.hpp"

#include <classad/classad.h>
#include <classad/classad_distribution.h>

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

using log4cplus::Logger;
using std::string;
using std::strstream;
using std::vector;

static Logger logger = Logger::getInstance("submitter");

bool ImageConfigI::CreateImageConfig(){
    //image_config_ad is not null
    if(image_config_ad.empty()){
        LOG4CPLUS_ERROR(logger, "create image faile,image_config_ad is null");
	return false;
    }
    //get rpc to create image config
    /*
    try {
           Proxy<MasterClient> proxy = Rpc<MasterClient, MasterClient>::GetProxy("localhost:9999");
	   image_id = proxy().CreateImage(image_config_ad);
           if(image_id == -1) {
               //std::cout<< "rpc error"<<std::endl;
	       LOG4CPLUS_ERROR(logger, "create image faile, master error");
               return false;
           }
       }  catch (TException &tx) {
          //std::cout<< "rpc error"<<std::endl;
	  LOG4CPLUS_ERROR(logger, "create image faile,rpc error");
          return false;
       }
    LOG4CPLUS_INFO(logger, "create image finished");
    */
    //std::cout<<image_id<<std::endl;
    return true;
}

bool ImageConfigI::DeleteImageConfig(const int32_t image_id){
    //get rpc to delete image config
    /*
    try {
           Proxy<MasterClient> proxy = Rpc<MasterClient, MasterClient>::GetProxy("localhost:9999");
           if(!proxy().DeleteImage(image_id)) {
               //std::cout<< "rpc error"<<std::endl;
               LOG4CPLUS_ERROR(logger, "delete image faile,rpc error");
               return false;
           }
       }  catch (TException &tx) {
          //std::cout<< "rpc error"<<std::endl;
          LOG4CPLUS_ERROR(logger, "delete image faile,rpc error");
          return false;
       }
    LOG4CPLUS_INFO(logger, "delete image finished");
    */
    return true;
}
/*
bool ImageConfigI::UpdateImageConfig(const string& image_config_ad){
    //image_config_ad is not null
    if(image_config_ad.empty()){
        LOG4CPLUS_ERROR(logger, "update image faile,image_config_ad is null");
        return false;
    }
    //get rpc to update image config
    try {
           Proxy<MasterClient> proxy = Rpc<MasterClient, MasterClient>::GetProxy("localhost:9999");
           if(!proxy().UpdateImage(image_config_ad)) {
               //std::cout<< "rpc error"<<std::endl;
               LOG4CPLUS_ERROR(logger, "update image faile,rpc error");
               return false;
           }
       }  catch (TException &tx) {
          //std::cout<< "rpc error"<<std::endl;
          LOG4CPLUS_ERROR(logger, "update image faile,rpc error");
          return false;
       }
    LOG4CPLUS_INFO(logger, "update image finished");
    return true;
}
*/

bool ImageConfigI::QueryImageConfig(const int32_t image_id){
    string image_config_ad;
    //get rpc to query image config
    try {
           Proxy<MasterClient> proxy = Rpc<MasterClient, MasterClient>::GetProxy("localhost:9999");
           proxy().QueryImage(image_config_ad, image_id);
               //std::cout<< "rpc error"<<std::endl;
               //LOG4CPLUS_ERROR(logger, "query image faile,rpc error");
               //return false;
       }  catch (TException &tx) {
          //std::cout<< "rpc error"<<std::endl;
          LOG4CPLUS_ERROR(logger, "query image faile,rpc error");
          return false;
       }
    if(image_config_ad.empty()){
	LOG4CPLUS_ERROR(logger, "query image faile, image_config_ad is null");
	return false; 
    }
    LOG4CPLUS_INFO(logger, "query image finished");

    //parse image_config_ad to php string that php can parse
    //init image_config 
    ImageConfig image_config;

    //get value from classad to image_config
    ClassAdParser parser;
    ClassAd* ad_ptr = parser.ParseClassAd(image_config_ad);

     //parse id
    if (!ad_ptr->EvaluateAttrNumber(ATTR_APP_ID, image_config.image_id)) {
        LOG4CPLUS_ERROR(logger, "Fails to init image, because parse " << ATTR_APP_ID << " error.");
        return false;
    }

      //parse name
    if (!ad_ptr->EvaluateAttrString(ATTR_APP_NAME, image_config.name)) {
        LOG4CPLUS_ERROR(logger, "Fails to init image, because parse " << ATTR_APP_NAME << " error.");
	return false;
    }

    //parse image_source
    if (!ad_ptr->EvaluateAttrString(ATTR_APP_SRC_PATH, image_config.image_source)) {
        LOG4CPLUS_ERROR(logger, "Fails to init image, because parse " << ATTR_APP_SRC_PATH << " error.");
	return false;
    }

    //parse install_dir
    if (!ad_ptr->EvaluateAttrString(ATTR_INSTALL_DIR, image_config.install_dir)) {
        LOG4CPLUS_ERROR(logger, "Fails to init image, because parse " << ATTR_INSTALL_DIR << " error.");
	return false;
    }


    /*   
    // put image_config to std out
    strstream ss;
    string image_id_str;
    ss << image_config.image_id;
    ss >> image_id_str;
    string image_config_php = "image_id="+image_id_str+"&"+"name="+image_config.name+"&"+"image_source="+image_config.image_source+"&"+"install_dir="+image_config.install_dir+"&"+"exe="+image_config.exe+"&"+"argument="+image_config.argument+"&"+"out_dir="+image_config.out_dir+"&"+"image_out_dir="+image_config.image_out_dir;
    std::cout << image_config_php << std::endl; 
    */
    return true;
}

bool ImageConfigI::QueryImageConfigList(){
    vector<string> image_config_ad_list;
    //get rpc to query image config
    try {
           Proxy<MasterClient> proxy = Rpc<MasterClient, MasterClient>::GetProxy("localhost:9999");
           proxy().QueryImageList(image_config_ad_list);
               //std::cout<< "rpc error"<<std::endl;
               //LOG4CPLUS_ERROR(logger, "query image faile,rpc error");
               //return false;
       }  catch (TException &tx) {
          std::cout<< "rpc error"<<tx.what()<<std::endl;
          LOG4CPLUS_ERROR(logger, "query image faile list,rpc error");
          return false;
       }
    if(image_config_ad_list.empty()){
        LOG4CPLUS_ERROR(logger, "query image list faile, image_config_ad_list is null");
        return false;
    }
    LOG4CPLUS_INFO(logger, "query image list finished");

    //parse image_config_ad to php string that php can parse
    /*
    for(int32_t i=0; i<image_config_ad_list.size(); i++){
        //init image_config 
        ImageConfig image_config;

        //get value from classad to image_config
        ClassAdParser parser;
        ClassAd* ad_ptr = parser.ParseClassAd(image_config_ad_list[i]);

        //parse id
        if (!ad_ptr->EvaluateAttrNumber(ATTR_APP_ID, image_config.image_id)) {
            LOG4CPLUS_ERROR(logger, "Fails to init image, because parse " << ATTR_APP_NAME << " error.");
            return false;
        }

        //parse name
        if (!ad_ptr->EvaluateAttrString(ATTR_APP_NAME, image_config.name)) {
            LOG4CPLUS_ERROR(logger, "Fails to init image, because parse " << ATTR_APP_NAME << " error.");
	    return false;
        }

        //parse image_source
        if (!ad_ptr->EvaluateAttrString(ATTR_APP_SRC_PATH, image_config.image_source)) {
            LOG4CPLUS_ERROR(logger, "Fails to init image, because parse " << ATTR_APP_SRC_PATH << " error.");
	    return false;
        } 

        //parse install_dir
        if (!ad_ptr->EvaluateAttrString(ATTR_INSTALL_DIR, image_config.install_dir)) {
            LOG4CPLUS_ERROR(logger, "Fails to init image, because parse " << ATTR_INSTALL_DIR << " error.");
	    return false;
        }

        // parse exe
        if (!ad_ptr->EvaluateAttrString(ATTR_EXE_PATH, image_config.exe)) {
            LOG4CPLUS_ERROR(logger, "Fails to init image, because parse " << ATTR_EXE_PATH << " error.");
	    return false;
        }

        // parse argument
        if (!ad_ptr->EvaluateAttrString(ATTR_ARGUMENT, image_config.argument)) {
            LOG4CPLUS_ERROR(logger, "Fails to init image, because parse " << ATTR_ARGUMENT << " error.");
	    return false;
        }

        // parse out_dir
        if (!ad_ptr->EvaluateAttrString(ATTR_OUT_DIR, image_config.out_dir)) {
            LOG4CPLUS_ERROR(logger, "Fails to init image, because parse " << ATTR_OUT_DIR << " error.");
	    return false;
        }

        //parse image_out_dir
        if (!ad_ptr->EvaluateAttrString(ATTR_APP_OUT_DIR, image_config.image_out_dir)) {
            LOG4CPLUS_ERROR(logger, "Fails to init image, because parse " << ATTR_APP_OUT_DIR << " error.");
	    return false;
        }
   
        // put image_config to std out
        strstream ss;
        string image_id_str;
        ss << image_config.image_id;
        ss >> image_id_str;
        string image_config_php = "image_id="+image_id_str+"&"+"name="+image_config.name+"&"+"image_source="+image_config.image_source+"&"+"install_dir="+image_config.install_dir+"&"+"exe="+image_config.exe+"&"+"argument="+image_config.argument+"&"+"out_dir="+image_config.out_dir+"&"+"image_out_dir="+image_config.image_out_dir;
        std::cout << image_config_php << std::endl; 
        }
    */
    return true;
}

bool ImageConfigI::QueryImageConfigAd(string& image_config_ad, const int32_t image_id){
     //get rpc to query image config
    try {
           Proxy<MasterClient> proxy = Rpc<MasterClient, MasterClient>::GetProxy("localhost:9999");
           proxy().QueryImage(image_config_ad, image_id);
               //std::cout<< "rpc error"<<std::endl;
               //LOG4CPLUS_ERROR(logger, "query image faile,rpc error");
               //return false;
       }  catch (TException &tx) {
          //std::cout<< "rpc error"<<std::endl;
          LOG4CPLUS_ERROR(logger, "query image faile,rpc error");
          return false;
       }
    if(image_config_ad.empty()){
        LOG4CPLUS_ERROR(logger, "query image faile, image_config_ad is null");
        return false;
    }
    LOG4CPLUS_INFO(logger, "query image finished");
    return true;
}


