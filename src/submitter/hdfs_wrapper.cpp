/********************************
 FileName: executor/hdfs_wrapper.cpp
 Author:   WangMin
 Date:     2013-11-13
 Version:  0.1
 Description: hdfs wrapper
*********************************/

#include "submitter/hdfs_wrapper.h"

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <gflags/gflags.h>

using log4cplus::Logger;

static Logger logger = Logger::getInstance("submitter");

DECLARE_string(hdfs_host);
DECLARE_int32(hdfs_port);


bool HDFSWrapper::Init() {
    m_local_fs = hdfsConnect(NULL, 0);
    if (NULL == m_local_fs) {
        LOG4CPLUS_ERROR(logger, "Failed to connect local fs");
        return false;
    }
    m_h_fs = hdfsConnect(FLAGS_hdfs_host.c_str(), FLAGS_hdfs_port);
    if (NULL == m_h_fs) {
        LOG4CPLUS_ERROR(logger, "Failed to connect hdfs, hdfs_host:" << FLAGS_hdfs_host << ", hdfs_port:" << FLAGS_hdfs_port);
        return false;
    }

    // test
    /*string local_path = "/tmp/";
    local_path += "output";
    string h_path = "/user/wm/output";
    if (!CopyToLocalFile(h_path, local_path)) {
        LOG4CPLUS_ERROR(logger, "Failed to copy file");
        return false;
    } 

    string local_path2 = "/home/wm/b.cpp";
    string h_path2 = "/user/root/b.cpp";
    if (!CopyFromLocalFile(local_path2, h_path2)) {
        LOG4CPLUS_ERROR(logger, "Failed to copy file");
        return false;
    }*/

    // close hdfs
    /* if (0 != hdfsDisconnect(local_fs)) {
        LOG4CPLUS_ERROR(logger, "Failed to disconnect local fs");
        return false;
    }
    if (0 != hdfsDisconnect(h_fs)) {
        LOG4CPLUS_ERROR(logger, "Failed to disconnect hdfs, hdfs_host:" << FLAGS_hdfs_host << ", hdfs_port:" << FLAGS_hdfs_port);
        return false;
    }*/
    return true; 
}

bool HDFSWrapper::CopyFromLocalFile(const string src, const string dst) {
    if (0 != hdfsCopy(m_local_fs, src.c_str(), m_h_fs, dst.c_str())) {
        LOG4CPLUS_ERROR(logger, "Failed to copy from local file, src: local file:" << src << ", dest: hdfs file:" << dst);    
        return false;
    }
    return true;
}

bool HDFSWrapper::CopyToLocalFile(const string src, const string dst) {
    if (0 != hdfsCopy(m_h_fs, src.c_str(), m_local_fs, dst.c_str())) {
        LOG4CPLUS_ERROR(logger, "Failed to copy to local file, src: hdfs file:" << src << ", dest: local file:" << dst);
        return false;
    }
    return true;
}

bool HDFSWrapper::DeleteHDFSFile(const string file_path){
    if(0 != hdfsDelete(m_h_fs, file_path.c_str())){
        LOG4CPLUS_ERROR(logger, "delete file on hdfs error");
        return false;
    }
    return true;
}
