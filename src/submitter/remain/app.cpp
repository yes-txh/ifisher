/*******************************
 File name: submitter/app.cpp
 Author:   Tangxuehai
 Date:     2013-11-18
 Version:  0.1
 Description: app 
*********************************/

#include <strstream>
#include <vector>
#include "submitter/app_config.h"
#include "submitter/app_file.h"
#include "submitter/app.h"
#include "include/classad_attr.h"

#include <classad/classad.h>
#include <classad/classad_distribution.h>

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

using log4cplus::Logger;
using std::string;
using std::strstream;
using std::vector;

static Logger logger = Logger::getInstance("submitter");

bool App::CreateApp(const string& app_config_ad){
    //create app config
    if(false == AppConfigPtrI::Instance()->CreateAppConfig(app_config_ad)){
	LOG4CPLUS_ERROR(logger, "create app faile");
        return false;
    }
    //create app file on hdfs
    string app_file_name;
    string app_file_source;
    string app_file_locate;
    //get value from classad to app_config
    ClassAdParser parser;
    ClassAd* ad_ptr = parser.ParseClassAd(app_config_ad);

    //parse app_file_name
    if (!ad_ptr->EvaluateAttrString(ATTR_APP_FILE_NAME, app_file_name)) {
        LOG4CPLUS_ERROR(logger, "Fails to init app, because parse " << ATTR_APP_FILE_NAME << " error.");
        return false;
    }

    //parse app_source
    if (!ad_ptr->EvaluateAttrString(ATTR_APP_SRC_PATH, app_file_source)) {
        LOG4CPLUS_ERROR(logger, "Fails to init app, because parse " << ATTR_APP_SRC_PATH << " error.");
        return false;
    }

    //parse install_dir
    if (!ad_ptr->EvaluateAttrString(ATTR_APP_LOCATE_PATH, app_file_locate)) {
        LOG4CPLUS_ERROR(logger, "Fails to init app, because parse " << ATTR_APP_LOCATE_PATH << " error.");
        return false;
    }
    
    if(app_file_name.empty() || app_file_source.empty() || app_file_locate.empty()){
	LOG4CPLUS_ERROR(logger, "Fails to create app, because parse classad is null");
        return false;
    }
    
    if(false == AppFilePtrI::Instance()->CreateAppFile(app_file_name, app_file_locate, app_file_source)){
        LOG4CPLUS_ERROR(logger, "create app file faile");
        return false;
    }
    return true;
}

bool App::DeleteApp(const int32_t app_id){
    //delete app config
    if(false == AppConfigPtrI::Instance()->DeleteAppConfig(app_id)){
        LOG4CPLUS_ERROR(logger, "delete app faile");
        return false;
    }
    //delete app file on hdfs
    string app_config_ad;
    if(false == AppConfigPtrI::Instance()->QueryAppConfigAd(app_config_ad, app_id)){
        LOG4CPLUS_ERROR(logger, "query app config ad faile");
        return false;
    }
    
     //create app file on hdfs
    string app_file_name;
    string app_file_install_dir;
    //get value from classad to app_config
    ClassAdParser parser;
    ClassAd* ad_ptr = parser.ParseClassAd(app_config_ad);

    //parse app_file_name
    if (!ad_ptr->EvaluateAttrString(ATTR_APP_FILE_NAME, app_file_name)) {
        LOG4CPLUS_ERROR(logger, "Fails to init app, because parse " << ATTR_APP_FILE_NAME << " error.");
        return false;
    }

    //parse install_dir
    if (!ad_ptr->EvaluateAttrString(ATTR_INSTALL_DIR, app_file_install_dir)) {
        LOG4CPLUS_ERROR(logger, "Fails to init app, because parse " << ATTR_INSTALL_DIR << " error.");
        return false;
    }

    if(app_file_name.empty() || app_file_install_dir.empty()){
        LOG4CPLUS_ERROR(logger, "Fails to create app, because parse classad is null");
        return false;
    }

    if(false == AppFilePtrI::Instance()->DeleteAppFile(app_file_name, app_file_install_dir)){
        LOG4CPLUS_ERROR(logger, "delete app file faile");
        return false;
    }
    return true;
}

bool App::UpdateApp(const string& app_config_ad){
    if(false == AppConfigPtrI::Instance()->UpdateAppConfig(app_config_ad)){
        LOG4CPLUS_ERROR(logger, "update app file faile");
        return false;
    }
    return true;
}

bool App::QueryApp(const int32_t app_id){
    if(false == AppConfigPtrI::Instance()->QueryAppConfig(app_id)){
        LOG4CPLUS_ERROR(logger, "query app  faile");
        return false;
    }
    return true;
}

bool App::QueryAppList(){
    if(false == AppConfigPtrI::Instance()->QueryAppConfigList()){
        LOG4CPLUS_ERROR(logger, "query app list faile");
        return false;
    }
    return true;
}


