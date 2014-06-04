/*******************************
 File name: submitter/app_config.cpp
 Author:   Tangxuehai
 Date:     2013-11-18
 Version:  0.1
 Description: app config 
*********************************/

#include <strstream>
#include <vector>
#include "submitter/app_config.h"
#include "include/classad_attr.h"
#include "include/proxy.h"
#include "common/clynn/rpc.h"

#include <classad/classad.h>
#include <classad/classad_distribution.h>

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

using log4cplus::Logger;
using std::string;
using std::strstream;
using std::vector;

static Logger logger = Logger::getInstance("submitter");

bool AppConfigI::CreateAppConfig(const string& app_config_ad){
    int32_t app_id;
    //app_config_ad is not null
    if(app_config_ad.empty()){
        LOG4CPLUS_ERROR(logger, "create app faile,app_config_ad is null");
	return false;
    }
    //get rpc to create app config
    try {
           Proxy<MasterClient> proxy = Rpc<MasterClient, MasterClient>::GetProxy("localhost:9999");
	   app_id = proxy().CreateApp(app_config_ad);
           if(app_id == -1) {
               //std::cout<< "rpc error"<<std::endl;
	       LOG4CPLUS_ERROR(logger, "create app faile, master error");
               return false;
           }
       }  catch (TException &tx) {
          //std::cout<< "rpc error"<<std::endl;
	  LOG4CPLUS_ERROR(logger, "create app faile,rpc error");
          return false;
       }
    LOG4CPLUS_INFO(logger, "create app finished");
    
    //std::cout<<app_id<<std::endl;
    return true;
}

bool AppConfigI::DeleteAppConfig(const int32_t app_id){
    //get rpc to delete app config
    try {
           Proxy<MasterClient> proxy = Rpc<MasterClient, MasterClient>::GetProxy("localhost:9999");
           if(!proxy().DeleteApp(app_id)) {
               //std::cout<< "rpc error"<<std::endl;
               LOG4CPLUS_ERROR(logger, "delete app faile,rpc error");
               return false;
           }
       }  catch (TException &tx) {
          //std::cout<< "rpc error"<<std::endl;
          LOG4CPLUS_ERROR(logger, "delete app faile,rpc error");
          return false;
       }
    LOG4CPLUS_INFO(logger, "delete app finished");
    return true;
}

bool AppConfigI::UpdateAppConfig(const string& app_config_ad){
    //app_config_ad is not null
    if(app_config_ad.empty()){
        LOG4CPLUS_ERROR(logger, "update app faile,app_config_ad is null");
        return false;
    }
    //get rpc to update app config
    try {
           Proxy<MasterClient> proxy = Rpc<MasterClient, MasterClient>::GetProxy("localhost:9999");
           if(!proxy().UpdateApp(app_config_ad)) {
               //std::cout<< "rpc error"<<std::endl;
               LOG4CPLUS_ERROR(logger, "update app faile,rpc error");
               return false;
           }
       }  catch (TException &tx) {
          //std::cout<< "rpc error"<<std::endl;
          LOG4CPLUS_ERROR(logger, "update app faile,rpc error");
          return false;
       }
    LOG4CPLUS_INFO(logger, "update app finished");
    return true;
}

bool AppConfigI::QueryAppConfig(const int32_t app_id){
    string app_config_ad;
    //get rpc to query app config
    try {
           Proxy<MasterClient> proxy = Rpc<MasterClient, MasterClient>::GetProxy("localhost:9999");
           proxy().QueryApp(app_config_ad, app_id);
               //std::cout<< "rpc error"<<std::endl;
               //LOG4CPLUS_ERROR(logger, "query app faile,rpc error");
               //return false;
       }  catch (TException &tx) {
          //std::cout<< "rpc error"<<std::endl;
          LOG4CPLUS_ERROR(logger, "query app faile,rpc error");
          return false;
       }
    if(app_config_ad.empty()){
	LOG4CPLUS_ERROR(logger, "query app faile, app_config_ad is null");
	return false; 
    }
    LOG4CPLUS_INFO(logger, "query app finished");

    //parse app_config_ad to php string that php can parse
    //init app_config 
    AppConfig app_config;

    //get value from classad to app_config
    ClassAdParser parser;
    ClassAd* ad_ptr = parser.ParseClassAd(app_config_ad);

     //parse id
    if (!ad_ptr->EvaluateAttrNumber(ATTR_APP_ID, app_config.app_id)) {
        LOG4CPLUS_ERROR(logger, "Fails to init app, because parse " << ATTR_APP_ID << " error.");
        return false;
    }

      //parse name
    if (!ad_ptr->EvaluateAttrString(ATTR_APP_NAME, app_config.name)) {
        LOG4CPLUS_ERROR(logger, "Fails to init app, because parse " << ATTR_APP_NAME << " error.");
	return false;
    }

    //parse app_source
    if (!ad_ptr->EvaluateAttrString(ATTR_APP_SRC_PATH, app_config.app_source)) {
        LOG4CPLUS_ERROR(logger, "Fails to init app, because parse " << ATTR_APP_SRC_PATH << " error.");
	return false;
    }

    //parse install_dir
    if (!ad_ptr->EvaluateAttrString(ATTR_INSTALL_DIR, app_config.install_dir)) {
        LOG4CPLUS_ERROR(logger, "Fails to init app, because parse " << ATTR_INSTALL_DIR << " error.");
	return false;
    }

    // parse exe
    if (!ad_ptr->EvaluateAttrString(ATTR_EXE_PATH, app_config.exe)) {
        LOG4CPLUS_ERROR(logger, "Fails to init app, because parse " << ATTR_EXE_PATH << " error.");
	return false;
    }

    // parse argument
    if (!ad_ptr->EvaluateAttrString(ATTR_ARGUMENT, app_config.argument)) {
        LOG4CPLUS_ERROR(logger, "Fails to init app, because parse " << ATTR_ARGUMENT << " error.");
	return false;
    }

    // parse out_dir
    if (!ad_ptr->EvaluateAttrString(ATTR_OUT_DIR, app_config.out_dir)) {
        LOG4CPLUS_ERROR(logger, "Fails to init app, because parse " << ATTR_OUT_DIR << " error.");
	return false;
    }

    //parse app_out_dir
    if (!ad_ptr->EvaluateAttrString(ATTR_APP_OUT_DIR, app_config.app_out_dir)) {
        LOG4CPLUS_ERROR(logger, "Fails to init app, because parse " << ATTR_APP_OUT_DIR << " error.");
	return false;
    }
   
    // put app_config to std out
    strstream ss;
    string app_id_str;
    ss << app_config.app_id;
    ss >> app_id_str;
    string app_config_php = "app_id="+app_id_str+"&"+"name="+app_config.name+"&"+"app_source="+app_config.app_source+"&"+"install_dir="+app_config.install_dir+"&"+"exe="+app_config.exe+"&"+"argument="+app_config.argument+"&"+"out_dir="+app_config.out_dir+"&"+"app_out_dir="+app_config.app_out_dir;
    std::cout << app_config_php << std::endl; 
    return true;
}

bool AppConfigI::QueryAppConfigList(){
    vector<string> app_config_ad_list;
    //get rpc to query app config
    try {
           Proxy<MasterClient> proxy = Rpc<MasterClient, MasterClient>::GetProxy("localhost:9999");
           proxy().QueryAppList(app_config_ad_list);
               //std::cout<< "rpc error"<<std::endl;
               //LOG4CPLUS_ERROR(logger, "query app faile,rpc error");
               //return false;
       }  catch (TException &tx) {
          std::cout<< "rpc error"<<tx.what()<<std::endl;
          LOG4CPLUS_ERROR(logger, "query app faile list,rpc error");
          return false;
       }
    if(app_config_ad_list.empty()){
        LOG4CPLUS_ERROR(logger, "query app list faile, app_config_ad_list is null");
        return false;
    }
    LOG4CPLUS_INFO(logger, "query app list finished");

    //parse app_config_ad to php string that php can parse
    
    for(int32_t i=0; i<app_config_ad_list.size(); i++){
        //init app_config 
        AppConfig app_config;

        //get value from classad to app_config
        ClassAdParser parser;
        ClassAd* ad_ptr = parser.ParseClassAd(app_config_ad_list[i]);

        //parse id
        if (!ad_ptr->EvaluateAttrNumber(ATTR_APP_ID, app_config.app_id)) {
            LOG4CPLUS_ERROR(logger, "Fails to init app, because parse " << ATTR_APP_NAME << " error.");
            return false;
        }

        //parse name
        if (!ad_ptr->EvaluateAttrString(ATTR_APP_NAME, app_config.name)) {
            LOG4CPLUS_ERROR(logger, "Fails to init app, because parse " << ATTR_APP_NAME << " error.");
	    return false;
        }

        //parse app_source
        if (!ad_ptr->EvaluateAttrString(ATTR_APP_SRC_PATH, app_config.app_source)) {
            LOG4CPLUS_ERROR(logger, "Fails to init app, because parse " << ATTR_APP_SRC_PATH << " error.");
	    return false;
        } 

        //parse install_dir
        if (!ad_ptr->EvaluateAttrString(ATTR_INSTALL_DIR, app_config.install_dir)) {
            LOG4CPLUS_ERROR(logger, "Fails to init app, because parse " << ATTR_INSTALL_DIR << " error.");
	    return false;
        }

        // parse exe
        if (!ad_ptr->EvaluateAttrString(ATTR_EXE_PATH, app_config.exe)) {
            LOG4CPLUS_ERROR(logger, "Fails to init app, because parse " << ATTR_EXE_PATH << " error.");
	    return false;
        }

        // parse argument
        if (!ad_ptr->EvaluateAttrString(ATTR_ARGUMENT, app_config.argument)) {
            LOG4CPLUS_ERROR(logger, "Fails to init app, because parse " << ATTR_ARGUMENT << " error.");
	    return false;
        }

        // parse out_dir
        if (!ad_ptr->EvaluateAttrString(ATTR_OUT_DIR, app_config.out_dir)) {
            LOG4CPLUS_ERROR(logger, "Fails to init app, because parse " << ATTR_OUT_DIR << " error.");
	    return false;
        }

        //parse app_out_dir
        if (!ad_ptr->EvaluateAttrString(ATTR_APP_OUT_DIR, app_config.app_out_dir)) {
            LOG4CPLUS_ERROR(logger, "Fails to init app, because parse " << ATTR_APP_OUT_DIR << " error.");
	    return false;
        }
   
        // put app_config to std out
        strstream ss;
        string app_id_str;
        ss << app_config.app_id;
        ss >> app_id_str;
        string app_config_php = "app_id="+app_id_str+"&"+"name="+app_config.name+"&"+"app_source="+app_config.app_source+"&"+"install_dir="+app_config.install_dir+"&"+"exe="+app_config.exe+"&"+"argument="+app_config.argument+"&"+"out_dir="+app_config.out_dir+"&"+"app_out_dir="+app_config.app_out_dir;
        std::cout << app_config_php << std::endl; 
        }
    return true;
}

bool AppConfigI::QueryAppConfigAd(string& app_config_ad, const int32_t app_id){
     //get rpc to query app config
    try {
           Proxy<MasterClient> proxy = Rpc<MasterClient, MasterClient>::GetProxy("localhost:9999");
           proxy().QueryApp(app_config_ad, app_id);
               //std::cout<< "rpc error"<<std::endl;
               //LOG4CPLUS_ERROR(logger, "query app faile,rpc error");
               //return false;
       }  catch (TException &tx) {
          //std::cout<< "rpc error"<<std::endl;
          LOG4CPLUS_ERROR(logger, "query app faile,rpc error");
          return false;
       }
    if(app_config_ad.empty()){
        LOG4CPLUS_ERROR(logger, "query app faile, app_config_ad is null");
        return false;
    }
    LOG4CPLUS_INFO(logger, "query app finished");
    return true;
}


