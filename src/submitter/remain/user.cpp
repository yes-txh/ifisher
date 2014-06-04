/*******************************
 File name: submitter/user.cpp
 Author:   Tangxuehai
 Date:     2013-11-21
 Version:  0.1
 Description: user 
*********************************/
#include <classad/classad.h>
#include <classad/classad_distribution.h>

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

#include <strstream>
#include <vector>
#include "submitter/user.h"
#include "include/classad_attr.h"
#include "include/proxy.h"
#include "common/clynn/rpc.h"

using log4cplus::Logger;
using std::string;
using std::strstream;
using std::vector;

static Logger logger = Logger::getInstance("submitter");

bool User::CreateUser(const string& user_config_ad){
    int32_t user_id;
    //app_config_ad is not null
    if(user_config_ad.empty()){
        LOG4CPLUS_ERROR(logger, "create user faile,user_config_ad is null");
	return false;
    }
    //get rpc to create app config
    try {
           Proxy<MasterClient> proxy = Rpc<MasterClient, MasterClient>::GetProxy("localhost:9999");
	   user_id = proxy().CreateUser(user_config_ad);
           if(user_id == -1) {
               //std::cout<< "rpc error"<<std::endl;
	       LOG4CPLUS_ERROR(logger, "create user faile, master error");
               return false;
           }
       }  catch (TException &tx) {
          //std::cout<< "rpc error"<<std::endl;
	  LOG4CPLUS_ERROR(logger, "create user faile,rpc error");
          return false;
       }
    LOG4CPLUS_INFO(logger, "create user finished");
    
    std::cout<<user_id<<std::endl;
    return true;
}

bool User::DeleteUser(const int32_t user_id){
    //get rpc to delete app config
    try {
           Proxy<MasterClient> proxy = Rpc<MasterClient, MasterClient>::GetProxy("localhost:9999");
           if(!proxy().DeleteUser(user_id)) {
               //std::cout<< "rpc error"<<std::endl;
               LOG4CPLUS_ERROR(logger, "delete user faile,rpc error");
               return false;
           }
       }  catch (TException &tx) {
          //std::cout<< "rpc error"<<std::endl;
          LOG4CPLUS_ERROR(logger, "delete user faile,rpc error");
          return false;
       }
    LOG4CPLUS_INFO(logger, "delete user finished");
    return true;
}

bool User::UpdateUser(const string& user_config_ad){
    //app_config_ad is not null
    if(user_config_ad.empty()){
        LOG4CPLUS_ERROR(logger, "update app faile,user_config_ad is null");
        return false;
    }
    //get rpc to update app config
    try {
           Proxy<MasterClient> proxy = Rpc<MasterClient, MasterClient>::GetProxy("localhost:9999");
           if(!proxy().UpdateUser(user_config_ad)) {
               //std::cout<< "rpc error"<<std::endl;
               LOG4CPLUS_ERROR(logger, "update user faile,rpc error");
               return false;
           }
       }  catch (TException &tx) {
          //std::cout<< "rpc error"<<std::endl;
          LOG4CPLUS_ERROR(logger, "update user faile,rpc error");
          return false;
       }
    LOG4CPLUS_INFO(logger, "update user finished");
    return true;
}

bool User::QueryUser(const int32_t user_id){
    string user_config_ad;
    //get rpc to query app config
    try {
           Proxy<MasterClient> proxy = Rpc<MasterClient, MasterClient>::GetProxy("localhost:9999");
           proxy().QueryUser(user_config_ad, user_id);
               //std::cout<< "rpc error"<<std::endl;
               //LOG4CPLUS_ERROR(logger, "query app faile,rpc error");
               //return false;
       }  catch (TException &tx) {
          //std::cout<< "rpc error"<<std::endl;
          LOG4CPLUS_ERROR(logger, "query user faile,rpc error");
          return false;
       }
    if(user_config_ad.empty()){
	LOG4CPLUS_ERROR(logger, "query user faile, user_config_ad is null");
	return false; 
    }
    LOG4CPLUS_INFO(logger, "query user finished");

    //parse app_config_ad to php string that php can parse
    //init app_config 
    UserInfo user_config;

    //get value from classad to app_config
    ClassAdParser parser;
    ClassAd* ad_ptr = parser.ParseClassAd(user_config_ad);

     //parse id
    if (!ad_ptr->EvaluateAttrNumber(ATTR_USER_ID, user_config.user_id)) {
        LOG4CPLUS_ERROR(logger, "Fails to init user, because parse " << ATTR_USER_ID << " error.");
        return false;
    }

      //parse name
    if (!ad_ptr->EvaluateAttrString(ATTR_USER_NAME, user_config.name)) {
        LOG4CPLUS_ERROR(logger, "Fails to init user, because parse " << ATTR_USER_NAME << " error.");
	return false;
    }

    //parse passwd
    if (!ad_ptr->EvaluateAttrString(ATTR_USER_PASSWD, user_config.passwd)) {
        LOG4CPLUS_ERROR(logger, "Fails to init user, because parse " << ATTR_USER_PASSWD << " error.");
	return false;
    }

    //parse is_admin
    if (!ad_ptr->EvaluateAttrBool(ATTR_USER_IS_ADMIN, user_config.is_admin)) {
        LOG4CPLUS_ERROR(logger, "Fails to init user, because parse " << ATTR_USER_IS_ADMIN << " error.");
	return false;
    }

    strstream ss;
    strstream sss;
    string user_id_str;
    string is_admin_str;
    ss << user_config.user_id;
    ss >> user_id_str;

    sss << user_config.is_admin;
    sss >> is_admin_str;

    string user_config_php = "user_id="+user_id_str+"&"+"name="+user_config.name+"&"+"passwd="+user_config.passwd+"&"+"is_admin="+is_admin_str;
    std::cout << user_config_php << std::endl; 
    return true;
}

bool User::QueryUserList(){
    vector<string> user_config_ad_list;
    //get rpc to query app config
    try {
           Proxy<MasterClient> proxy = Rpc<MasterClient, MasterClient>::GetProxy("localhost:9999");
           proxy().QueryUserList(user_config_ad_list);
               //std::cout<< "rpc error"<<std::endl;
               //LOG4CPLUS_ERROR(logger, "query app faile,rpc error");
               //return false;

       }  catch (TException &tx) {
          std::cout<< "rpc error"<<tx.what()<<std::endl;
          LOG4CPLUS_ERROR(logger, "query  user list faile,rpc error");
          return false;
       }
    if(user_config_ad_list.empty()){
        LOG4CPLUS_ERROR(logger, "query user list faile, app_config_ad_list is null");
        return false;
    }
    LOG4CPLUS_INFO(logger, "query user list finished");

    //parse app_config_ad to php string that php can parse
    
    for(int32_t i=0; i<user_config_ad_list.size(); i++){
        //init app_config 
    UserInfo user_config;

    //get value from classad to app_config
    ClassAdParser parser;
    ClassAd* ad_ptr = parser.ParseClassAd(user_config_ad_list[i]);

     //parse id
    if (!ad_ptr->EvaluateAttrNumber(ATTR_USER_ID, user_config.user_id)) {
        LOG4CPLUS_ERROR(logger, "Fails to init user, because parse " << ATTR_USER_ID << " error.");
        return false;
    }

      //parse name
    if (!ad_ptr->EvaluateAttrString(ATTR_USER_NAME, user_config.name)) {
        LOG4CPLUS_ERROR(logger, "Fails to init user, because parse " << ATTR_USER_NAME << " error.");
        return false;
    }

    //parse passwd
    if (!ad_ptr->EvaluateAttrString(ATTR_USER_PASSWD, user_config.passwd)) {
        LOG4CPLUS_ERROR(logger, "Fails to init user, because parse " << ATTR_USER_PASSWD << " error.");
        return false;
    }

    //parse is_admin
    if (!ad_ptr->EvaluateAttrBool(ATTR_USER_IS_ADMIN, user_config.is_admin)) {
        LOG4CPLUS_ERROR(logger, "Fails to init user, because parse " << ATTR_USER_IS_ADMIN << " error.");
        return false;
    }

    strstream ss;
    strstream sss;
    string user_id_str;
    string is_admin_str;
    ss << user_config.user_id;
    ss >> user_id_str;

    sss << user_config.is_admin;
    sss >> is_admin_str;

    string user_config_php = "user_id="+user_id_str+"&"+"name="+user_config.name+"&"+"passwd="+user_config.passwd+"&"+"is_admin="+is_admin_str;
    std::cout << user_config_php << std::endl;
    }   
    return true;
}

