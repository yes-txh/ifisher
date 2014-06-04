/********************************
 FileName: submitter/config.cpp
 Author:   Tangxuehai
 Date:     2013-11-19
 Version:  0.1
 Description: config for submitter
*********************************/

#include "gflags/gflags.h"

DEFINE_string(JobsManager_endpoint, "127.0.0.1:9999", "JobsManager endpoint");
DEFINE_string(xml_path, "", "xml path");
DEFINE_int32(hdfs_port, 9000, "hdfs port");
DEFINE_string(hdfs_host, "localhost", "hdfs host");
DEFINE_bool(first_create_image, true, "image is first create or not");

DEFINE_string(object, "Image", "JobManager endpoint");
DEFINE_string(operation, "create", "JobManager endpoint");
DEFINE_int32(job_id, 0, "job id");
//DEFINE_int32(user_id, 0, "hdfs port");

//DEFINE_string(lxc_template, "/var/lib/lynn/lxc/", "default lxc conf template");

// DEFINE_string(work_directory, "/tmp/cello", "cellet work directory");
// DEFINE_string(dfs_ip, "", "distributed file system server ip");
// DEFINE_int32(dfs_port, 0, "distributed file system server port");
// DEFINE_string(policy_file, "", "policy configuration file");
