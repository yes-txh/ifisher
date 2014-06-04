/********************************
 File name: appmgr/app.h
 Author:   Tangxuehai
 Date:     2014-04-23
 Version:  0.1
 Description: app 
*********************************/

#ifndef SRC_SUBMITTER_App_H
#define SRC_SUBMITTER_App_H

#include <string>
#include <boost/shared_ptr.hpp>

#include "include/proxy.h"
#include "common/clynn/singleton.h"
//#include "common/clynn/rwlock.h"
//#include "common/clynn/rpc.h"

class App{
public:
     bool CreateApp(const string& xml_path);
     //bool DeleteImage(const int32_t image_id);
     //bool QueryImage(const int32_t image_id);
     //bool QueryImageList();

private:
     bool Init();
     bool SendApptoHDFS();

     string m_name;
     string m_user;
     string m_local_path;
     string m_xml_path;
     string m_exe;
};
typedef Singleton<App> AppPtrI;
#endif


