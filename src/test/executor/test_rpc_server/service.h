/********************************
 FileName: executor/service.h
 Author:   WangMin
 Date:     2013-08-14
 Version:  0.1
 Description: executor service
*********************************/

#ifndef SRC_EXECUTOR_SERVICE_H
#define SRC_EXECUTOR_SERVICE_H

#include "include/proxy.h"

using std::string;

class TestService : public TestIf {
public:
    int32_t  Helloworld();

};

#endif
