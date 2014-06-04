/********************************
 File name: submitter/app.h
 Author:   Tangxuehai
 Date:     2013-11-18
 Version:  0.1
 Description: app 
*********************************/

#ifndef SRC_SUBMITTER_APP_H
#define SRC_SUBMITTER_APP_H

#include <string>
#include <boost/shared_ptr.hpp>

#include "include/proxy.h"
#include "common/clynn/singleton.h"
//#include "common/clynn/rwlock.h"
//#include "common/clynn/rpc.h"

class App{
public:
     bool CreateApp(const string& app_config_ad);
     bool DeleteApp(const int32_t app_id);
     bool UpdateApp(const string& app_config_ad);
     bool QueryApp(const int32_t app_id);
     bool QueryAppList();
};
typedef Singleton<App> AppPtrI;
#endif


