/********************************
 File name: jobmgr/job.h
 Author:   Tangxuehai
 Date:     2014-04-24
 Version:  0.1
 Description: app 
*********************************/

#ifndef SRC_SUBMITTER_Job_H
#define SRC_SUBMITTER_Job_H

#include <string>
#include <boost/shared_ptr.hpp>

#include "include/proxy.h"
#include "common/clynn/singleton.h"

class Job{
public:
     bool CreateJob(const string& xml_path);
     //bool DeleteImage(const int32_t image_id);
     bool QueryJob(const int32_t job_id);
     //bool QueryImageList();

};
typedef Singleton<Job> JobPtrI;
#endif


