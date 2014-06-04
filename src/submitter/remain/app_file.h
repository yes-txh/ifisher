/********************************
 File name: submitter/app_file.h
 Author:   Tangxuehai
 Date:     2013-11-18
 Version:  0.1
 Description: app file
*********************************/

#ifndef SRC_SUBMITTER_APP_FILE_H
#define SRC_SUBMITTER_APP_FILE_H

#include <string>
#include <boost/shared_ptr.hpp>

//#include "include/proxy.h"
#include "common/clynn/singleton.h"
//#include "common/clynn/rpc.h"

class AppFile{
public:
     bool CreateAppFile(const string& app_file_name, const string& app_file_locate, const string& app_file_source);
     bool DeleteAppFile(const string& app_file_name, const string& app_file_source);
};
typedef Singleton<AppFile> AppFilePtrI;
#endif


