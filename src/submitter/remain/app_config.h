/********************************
 File name: submitter/app_config.h
 Author:   Tangxuehai
 Date:     2013-11-18
 Version:  0.1
 Description: app config 
*********************************/

#ifndef SRC_SUBMITTER_APP_CONFIG_H
#define SRC_SUBMITTER_APP_CONFIG_H

#include <string>
#include <boost/shared_ptr.hpp>

#include "include/proxy.h"
#include "common/clynn/rwlock.h"
#include "common/clynn/rpc.h"
#include "common/clynn/singleton.h"

class AppConfigI{
public:
     bool CreateAppConfig(const string& app_config_ad);
     bool DeleteAppConfig(const int32_t app_id);
     bool UpdateAppConfig(const string& app_config_ad);
     bool QueryAppConfig(const int32_t app_id);
     bool QueryAppConfigList();
     bool QueryAppConfigAd(string& app_config_ad, const int32_t app_id);
};
typedef Singleton<AppConfigI> AppConfigPtrI;
#endif


