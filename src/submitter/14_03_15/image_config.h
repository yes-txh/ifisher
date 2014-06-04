/********************************
 File name: submitter/image_config.h
 Author:   Tangxuehai
 Date:     2013-11-18
 Version:  0.1
 Description: image config 
*********************************/

#ifndef SRC_SUBMITTER_APP_CONFIG_H
#define SRC_SUBMITTER_APP_CONFIG_H

#include <string>
#include <map>
#include <boost/shared_ptr.hpp>

#include "include/proxy.h"
#include "common/clynn/rwlock.h"
#include "common/clynn/rpc.h"
#include "common/clynn/singleton.h"

class ImageConfigI{
public:
     bool Init(const string& conf_file);
     string Get(const string& name);
     bool CreateImageConfig();
     //bool DeleteImageConfig(const int32_t image_id);
     //bool QueryImageConfig(const int32_t image_id);
     //bool QueryImageConfigList();
     //bool QueryImageConfigAd(string& image_config_ad, const int32_t image_id);
private:
     map<string, string> m_attr_map;
};
typedef Singleton<ImageConfigI> ImageConfigPtrI;
#endif


