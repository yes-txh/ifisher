/********************************
 File name: submitter/image_file.h
 Author:   Tangxuehai
 Date:     2013-11-18
 Version:  0.1
 Description: image file
*********************************/

#ifndef SRC_SUBMITTER_IMAGE_FILE_H
#define SRC_SUBMITTER_IMAGE_FILE_H

#include <string>
#include <boost/shared_ptr.hpp>

//#include "include/proxy.h"
#include "common/clynn/singleton.h"
//#include "common/clynn/rpc.h"

class ImageFile{
public:
     bool CreateImageFile(const string& image_name, const string& image_user, const string& image_local_path);
     bool DeleteImageFile(const string& image_name, const string& image_user);
};
typedef Singleton<ImageFile> ImageFilePtrI;
#endif

