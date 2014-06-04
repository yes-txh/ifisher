/********************************
 FileName: executor/image_maneger.h
 Author:   Tangxuehai
 Date:     2014-03-16
 Version:  0.1
 Description: image manager
*********************************/

#ifndef JOB_MANAGER_IMAGE_MANAGER_H
#define JOB_MANAGER_IMAGE_MANAGER_H

#include <string>
#include <boost/shared_ptr.hpp>

#include "common/clynn/singleton.h"

class ImageManager {
public:
    bool UpdateImage(const string& user, const string& name, const int32_t size, bool is_update_all);

};

typedef Singleton<ImageManager> ImageMgrI;

#endif  
