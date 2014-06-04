/********************************
 FileName: executor/image_maneger.h
 Author:   Tangxuehai
 Date:     2014-03-16
 Version:  0.1
 Description: image manager
*********************************/

#ifndef SRC_EXECUTOR_IMAGE_MANAGER_H
#define SRC_EXECUTOR_IMAGE_MANAGER_H

#include <string>
#include <boost/shared_ptr.hpp>

#include "common/clynn/singleton.h"

class ImageManager {
public:
    bool ExecutorUpdateImage(const string& image_user, const string& image_name, const int32_t image_size);
    bool CheckImageAttrFromFile(const string& image);
    string GetImageAttrFromFile();

private:
    string m_name;
    string m_user;
    int32_t m_size;
};

typedef Singleton<ImageManager> ImageMgrI;

#endif  
