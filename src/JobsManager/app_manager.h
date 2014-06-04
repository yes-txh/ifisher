/********************************
 FileName: executor/app_maneger.h
 Author:   Tangxuehai
 Date:     2014-04-23
 Version:  0.1
 Description: app manager
*********************************/

#ifndef JOB_MANAGER_APP_MANAGER_H
#define JOB_MANAGER_APP_MANAGER_H

#include <string>
#include <map>
#include <boost/shared_ptr.hpp>

#include "common/clynn/singleton.h"
#include "common/clynn/rwlock.h"

using std::map;
using clynn::RWLock;

class AppManager {
public:
    bool CreateApp(const string& user, const string& name);

private:
   RWLock app_name_to_user_lock;
   map<string, string> app_name_to_user;
};

typedef Singleton<AppManager> AppMgrI;

#endif  
