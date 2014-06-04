/********************************
 FileName: include/classad_attr.h
 Author:   WangMin
 Date:     2013-09-16
 Version:  0.1
 Description: attributes of classad
*********************************/

#ifndef INCLUDE_CLASSAD_ATTR_H
#define INCLUDE_CLASSAD_ATTR_H
#include <string>

using std::string;

/// @brief: TaskInfo
// task info overview
static const string ATTR_JOB_ID = "JOB_ID";
static const string ATTR_TASK_ID = "TASK_ID";
static const string ATTR_VMTYPE = "VMTYPE";
static const string ATTR_IS_RUN = "IS_RUN";
static const string ATTR_USER = "USER";

// task vminfo
static const string ATTR_MEMORY = "MEMORY";
static const string ATTR_VCPU = "VCPU";
static const string ATTR_IP = "IP";
static const string ATTR_PORT = "PORT";
static const string ATTR_OS = "OS";
// -- only for kvm
static const string ATTR_IMG = "IMG";
static const string ATTR_ISO = "ISO";
static const string ATTR_SIZE = "SIZE";
static const string ATTR_VNC_PORT = "VNC_PORT";
static const string ATTR_RPC_RUNNING = "RPC_RUNNING";

// task appinfo
static const string ATTR_APP_ID = "APP_ID";
static const string ATTR_APP_NAME = "APP_NAME";
static const string ATTR_APP_FILE = "APP_FILE";

// -- outside vm, hdfs
static const string ATTR_APP_SRC_PATH = "SRC_PATH";
static const string ATTR_APP_OUT_DIR = "OUT_DIR";
// -- inside vm, windows or linux
static const string ATTR_INSTALL_DIR = "INSTALL_DIR";
static const string ATTR_EXE_PATH = "EXE_PATH";
static const string ATTR_ARGUMENT = "ATTR_ARGUMENT";
static const string ATTR_STOP_PATH = "STOP_PATH";
static const string ATTR_OUT_DIR = "EXE_OUT_DIR";
static const string ATTR_EXE = "EXE";


/// @brief: Register MachineInfo
static const string ATTR_Machine = "Machine";
static const string ATTR_MachineType = "MachineType";
static const string ATTR_Shelf = "Shelf";
static const string ATTR_Machine_IP = "IP";
static const string ATTR_Port = "Port";
static const string ATTR_Arch = "Arch";
static const string ATTR_OpSys = "OpSys";
static const string ATTR_TotalCPUNum = "TotalCPUNum";
static const string ATTR_TotalMemory = "TotalMemory";
static const string ATTR_TotalDisk = "TotalDisk";
static const string ATTR_AvailCPUNum = "AvailCPUNum";
static const string ATTR_AvailMemory = "AvailMemory";
static const string ATTR_AvailDisk = "AvailDisk";
static const string ATTR_BandWidth = "BandWidth";
static const string ATTR_NICType = "NICType";
static const string ATTR_LOAD_AVG = "LoadAvg";
static const string ATTR_Image = "Image";

static const string ATTR_AVAIL_CPU = "AvailCPUNum";
static const string ATTR_AVAIL_MEMORY = "AvailMemory";


static const string ATTR_JOB_TYPE = "JobType";
static const string ATTR_GROUP = "Group";
static const string ATTR_PRIO = "Priority";
static const string ATTR_SCHED_MODEL = "SchedModel";
static const string ATTR_TOTAL_TASKS = "TotalTasks";
static const string ATTR_NEED_CPU = "NeedCPU";
static const string ATTR_NEED_MEMORY = "NeedMemory";
static const string ATTR_TIMEOUT = "Timeout";
// static const string ATTR_EXE_PATH = "Exepath";
static const string ATTR_JOB_RANK = "JobRank";
static const string ATTR_JOB_REQUIREMENT = "JobRequirement";

static const string ATTR_MACHINE_STATUS = "MachineStatus";
static const string ATTR_TASK_STATUS = "TaskStatus";
static const string ATTR_TASK_START_TIME = "TaskStartTime";

static const string ATTR_TASK_RANK = "TaskRank";
static const string ATTR_TASK_REQUIREMENT = "TaskRequirement";
static const string ATTR_HARD_REQUIREMENT = "HardRequirement";
static const string ATTR_SOFT_REQUIREMENT = "SoftRequirement";
static const string ATTR_SOFT_VALUE = "SoftValue";


static const string EXP_TASK_RANK = ATTR_VCPU + "/" + ATTR_AvailCPUNum + " > 1.0*" + ATTR_MEMORY + "/" + ATTR_AvailMemory + " ? " + ATTR_VCPU + "/" + ATTR_AvailCPUNum + ": 1.0*" + ATTR_MEMORY+ "/" + ATTR_AvailMemory;
static const string EXP_TASK_REQUIREMENT = ATTR_VCPU + " <=" + ATTR_AvailCPUNum  + "&&" +  ATTR_MEMORY + "<=" +  ATTR_AvailMemory;

//image info
static const string ATTR_IMAGE_NAME = "IMAGE_NAME";
static const string ATTR_IMAGE_SIZE = "IMAGE_SIZE";
static const string ATTR_IMAGE_USER = "IMAGE_USER";

//vm hb
static const string ATTR_VMHB_CPU = "VMHB_CPU";
static const string ATTR_VMHB_MEMORY = "VMHB_MEMORY";
static const string ATTR_VMHB_BYTES_IN = "VMHB_BYTES_IN";
static const string ATTR_VMHB_BYTES_OUT = "VMHB_BYTES_OUT";
static const string ATTR_VMHB_STATE = "VMHB_STATE";

static const string ATTR_JOB_ID_RS = "JOB_ID_RS";
static const string ATTR_TASK_ID_RS = "TASK_ID_RS";


#endif
