/*******************************
 File name: submitter/app_file.cpp
 Author:   Tangxuehai
 Date:     2013-11-18
 Version:  0.1
 Description: app file
*********************************/

#include <fstream>
#include <iostream>
#include <sstream>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

#include "submitter/app_file.h"
#include "include/hdfs.h"

using log4cplus::Logger;
using std::string;
using std::ifstream;
using std::ios;
using std::ostringstream;

static Logger logger = Logger::getInstance("submitter");

bool AppFile::CreateAppFile(const string& app_file_name, const string& app_file_locate, const string& app_file_source){
    //get endpoint from zookeeper 

    //connect hdfs
    hdfsFS fs = hdfsConnect("localhost",9000 );
    if(!fs) {
	LOG4CPLUS_ERROR(logger, "connect hdfs error");
	return false;
    }
    std::cout<<"hello world 1"<<std::endl;
    //open hdfs file handler
    string write_file = app_file_source + "/" + app_file_name;
    //std::cout<<write_file<<std::endl;
    //string write_file = "/tmp/a.txt";
    hdfsFile writeFile = hdfsOpenFile(fs, write_file.c_str(), O_WRONLY|O_CREAT, 0, 0, 0);
    if(!writeFile) {
	LOG4CPLUS_ERROR(logger, "Failed to open for writing!");
        return false;
    }
    std::cout<<"hello world 2"<<std::endl;
    //write hdfs file
    // read local file to stream
    ostringstream buf;
    char ch;
    string app_file;
    string app_file_sn = app_file_locate + "/" + app_file_name;
    ifstream ifile(app_file_sn.c_str());

    while(buf&&ifile.get(ch)){
        buf.put(ch);
    }
    app_file = buf.str();
    std::cout<<app_file<<std::endl;
    tSize num_written_bytes = hdfsWrite(fs, writeFile, app_file.c_str(), app_file.size());
    if(num_written_bytes == -1){
        LOG4CPLUS_ERROR(logger, "Failed to write");
        return false;
    }
    std::cout<<"hello world "<<num_written_bytes<<std::endl;

    if (hdfsFlush(fs, writeFile)) {
        LOG4CPLUS_ERROR(logger, "Failed to flush");
        return false;
    }

    hdfsCloseFile(fs, writeFile);
    return true;
}

bool AppFile::DeleteAppFile(const string& app_file_name, const string& app_file_source){
     //get endpoint from zookeeper 

    //connect hdfs
    hdfsFS fs = hdfsConnect("default", 0);
    if(!fs) {
        LOG4CPLUS_ERROR(logger, "connect hdfs error");
        return false;
    }
    //delete file on hdfs
    string delete_file = app_file_source + app_file_name;
    if(hdfsDelete(fs, delete_file.c_str()) != 0){
	LOG4CPLUS_ERROR(logger, "delete file on hdfs error");
        return false;
    }
    return true;
}
