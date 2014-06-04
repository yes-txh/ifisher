/********************************
 File name: submitter/user.h
 Author:   Tangxuehai
 Date:     2013-11-21
 Version:  0.1
 Description: user
*********************************/

#ifndef SRC_SUBMITTER_USER_H
#define SRC_SUBMITTER_USER_H

#include <string>
#include <boost/shared_ptr.hpp>

#include "include/proxy.h"
#include "common/clynn/singleton.h"
//#include "common/clynn/rwlock.h"
//#include "common/clynn/rpc.h"

class User{
public:
     bool CreateUser(const string& app_config_ad);
     bool DeleteUser(const int32_t app_id);
     bool UpdateUser(const string& app_config_ad);
     bool QueryUser(const int32_t app_id);
     bool QueryUserList();
};
typedef Singleton<User> UserPtrI;
#endif


