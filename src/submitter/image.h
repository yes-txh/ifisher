/********************************
 File name: imgmgr/img.h
 Author:   Tangxuehai
 Date:     2014-03-12
 Version:  0.1
 Description: Image 
*********************************/

#ifndef SRC_SUBMITTER_Image_H
#define SRC_SUBMITTER_Image_H

#include <string>
#include <boost/shared_ptr.hpp>

#include "include/proxy.h"
#include "common/clynn/singleton.h"
//#include "common/clynn/rwlock.h"
//#include "common/clynn/rpc.h"

class Image{
public:
     bool CreateImage(const string& xml_path);
     //bool DeleteImage(const int32_t image_id);
     //bool QueryImage(const int32_t image_id);
     //bool QueryImageList();

private:
     bool Init();
     bool SendImagetoHDFS();

     string m_name;
     string m_user;
     string m_local_path;
     string m_xml_path;
     int32_t m_size;
     bool m_is_update_all;
};
typedef Singleton<Image> ImagePtrI;
#endif


