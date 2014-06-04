/********************************
 FileName: executor/kvm.cpp
 Author:   WangMin & TangXuehai
 Date:     2013-08-27 & 2014-05-10
 Version:  0.1
 Description: kvm, inherit from vm
*********************************/
  
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <gflags/gflags.h>

#include "common/rapidxml/rapidxml.hpp"
#include "common/rapidxml/rapidxml_utils.hpp"
#include "common/rapidxml/rapidxml_print.hpp"
#include "common/clynn/rpc.h"

#include <classad/classad.h>
#include <classad/classad_distribution.h>
#include "include/classad_attr.h"

#include "include/proxy.h"
#include "executor/hdfs_wrapper.h"
#include "executor/kvm.h"
#include "executor/task_entity_pool.h"
#include "executor/resource_manager.h"
#include "executor/dispatcher.h"
#include "executor/image_manager.h"
#include "executor/vm_pool.h"

using std::cout;
using std::stringstream;
using std::ifstream;
using std::ofstream;
using std::endl;
using namespace rapidxml;
using namespace std;
using log4cplus::Logger;
using clynn::WriteLocker;
using clynn::ReadLocker;

// gflag 
DECLARE_string(libvirt_dir);
DECLARE_string(xml_template);
DECLARE_int32(vm_hb_interval);
DECLARE_string(hdfs_host);
DECLARE_int32(hdfs_port);
DECLARE_bool(debug);
DECLARE_int32(starting_timeout);

static Logger logger = Logger::getInstance("executor");

string KVM::m_xml_template = "";
virConnectPtr KVM::m_conn = NULL;


/// @brief: public function
// KVM & ~KVM are inplement in kvm.h

/// virtual function, from V
// virtual C, include CreateVM and Install 
int32_t KVM::Execute() {
    if (CreateVM() != 0) {
        LOG4CPLUS_ERROR(logger, "Failed to create kvm, name:" << GetName() << ", job_id:" << GetID().job_id << ", task_id:" << GetID().task_id); 
        return -1;
    }
    return 0;
}

// create vm
int32_t KVM::CreateVM() {
    if (CreateKVM() != 0) {
        LOG4CPLUS_ERROR(logger, "Failed to create kvm, name:" << GetName() << ", job_id:" << GetID().job_id << ", task_id:" << GetID().task_id);
        return -1;
    }
    
    if (SetVNetByXML() != 0) {
        LOG4CPLUS_ERROR(logger, "Failed to set vnet by xml, name:" << GetName() << ", job_id:" << GetID().job_id << ", task_id:" << GetID().task_id);
        return -1;
    }
    return 0;
}

bool KVM::Stop() {
    return true;
}

bool KVM::Kill() {
    if(FLAGS_debug){
        LOG4CPLUS_DEBUG(logger, "Kill KVM, name:" << GetName());
        // delete work dir
        ifstream in_file(m_dir.c_str(), ios::in);

        //open file error ?
        if(in_file){
            string cmd = "rm -r " + m_dir;
            system(cmd.c_str());
        }
	return true;
    }
    if (!m_domain_ptr) {
        LOG4CPLUS_ERROR(logger, "Invalid domain pointer, Kill VM directly.");
        // delete work dir
        string cmd = "rm -r " + m_dir;
        system(cmd.c_str());
        return true;
    }

    if (virDomainDestroy(m_domain_ptr) != 0) {
        virErrorPtr error = virGetLastError();
        LOG4CPLUS_ERROR(logger, error->message);
        LOG4CPLUS_ERROR(logger, "Can't kill kvm, name:" << GetName() << ", job_id:" << GetID().job_id << ", task_id:" << GetID().task_id);
        return true;
    }

    // delete work dir
     // delete work dir
    ifstream in_file(m_dir.c_str(), ios::in);
    
    //open file error ?
    if(in_file){
        string cmd = "rm -r " + m_dir;
        system(cmd.c_str());
    }
    return true;
}

HbVMInfo KVM::GetHbVMInfo(){
    TaskID id = GetID();
    TaskPtr task_ptr = GetTaskPtr();
    TaskEntityState::type task_state = task_ptr->GetState();
    if (task_state == TaskEntityState::TASKENTITY_WAITING || task_state == TaskEntityState::TASKENTITY_STARTING){
	if(time(NULL) - m_starting_time > FLAGS_starting_timeout) {
	    //set vm state is false
	    VMPoolI::Instance()->SetVMStateByTaskID(id, false);
	    // new timeoutActionEvent
            EventPtr event(new TimeoutTaskEvent(id));
            // Push event into Queue
            EventDispatcherI::Instance()->Dispatch(event->GetType())->PushBack(event);
	}
        HbVMInfo empty;
        empty.id = GetID();
        empty.cpu_usage = 0;
        empty.memory_usage = 0;
        empty.bytes_in = 0;
        empty.bytes_out = 0;
        if(task_state == TaskEntityState::TASKENTITY_WAITING) {
              empty.app_state = AppState::APP_WAITING;
        } else {
              empty.app_state = AppState::APP_STARTING;
        }
	return empty;
    } else if(task_state == TaskEntityState::TASKENTITY_FAILED || task_state == TaskEntityState::TASKENTITY_FINISHED) {
	ReadLocker locker(m_lock);
        return m_hb_vm_info;
    } else if(task_state == TaskEntityState::TASKENTITY_RUNNING) {
	int32_t timeout = GetTaskInfo().timeout;
	if(time(NULL) - m_running_time > timeout) {
	    HbVMInfo empty;
            empty.id = GetID();
            empty.cpu_usage = 0;
            empty.memory_usage = 0;
            empty.bytes_in = 0;
            empty.bytes_out = 0;
            empty.app_state = AppState::APP_MISSED;
	    //set vm state is false
            VMPoolI::Instance()->SetVMStateByTaskID(id, false);
            // update task  state into missed
            // new timeoutActionEvent
            EventPtr event(new TimeoutTaskEvent(id));
            // Push event into Queue
            EventDispatcherI::Instance()->Dispatch(event->GetType())->PushBack(event);
            return empty;

	}

	if ((m_timestamp != -1) && (time(NULL) - m_timestamp > m_time_to_death)) {
            HbVMInfo empty;
            empty.id = GetID();
            empty.cpu_usage = 0;
            empty.memory_usage = 0;
            empty.bytes_in = 0;
            empty.bytes_out = 0;
            empty.app_state = AppState::APP_MISSED;
	    //set vm state is false
            VMPoolI::Instance()->SetVMStateByTaskID(id, false);

            // update task  state into missed
            // new missedActionEvent
            EventPtr event(new MissedTaskEvent(id));
            // Push event into Queue
            EventDispatcherI::Instance()->Dispatch(event->GetType())->PushBack(event);
            return empty;
         } else {
	    ReadLocker locker(m_lock);
	    return m_hb_vm_info;
         } 
    }else {
	ReadLocker locker(m_lock);
	return m_hb_vm_info;
    }

}

void KVM::SetHbVMInfo(const string& hb_vm_info_ad) {
    WriteLocker locker(m_lock);
    // classad init, string vmhb_info --> ClassAd *ad_ptr

    if(m_running_time == -1) {
	m_running_time = time(NULL);
    }
    m_timestamp = time(NULL);

    ClassAdParser parser;
    ClassAd* ad_ptr = parser.ParseClassAd(hb_vm_info_ad);

    if (!ad_ptr) {
        LOG4CPLUS_ERROR(logger, "Failed to parse classad, ClassAd pointer is NULL.");
        return;
    }

    if (!ad_ptr->EvaluateAttrNumber(ATTR_JOB_ID, m_hb_vm_info.id.job_id)) {
        LOG4CPLUS_ERROR(logger, "Parse " << ATTR_JOB_ID << " error.");
        return;
    }
    if (!ad_ptr->EvaluateAttrNumber(ATTR_TASK_ID, m_hb_vm_info.id.task_id)) {
        LOG4CPLUS_ERROR(logger, "Parse " << ATTR_TASK_ID << " error.");
        return;
    }
    
    if (!ad_ptr->EvaluateAttrNumber(ATTR_VMHB_CPU, m_hb_vm_info.cpu_usage)) {
        LOG4CPLUS_ERROR(logger, "Parse " << ATTR_VMHB_CPU << " error.");
        return;
    }
   
    if (!ad_ptr->EvaluateAttrNumber(ATTR_VMHB_MEMORY, m_hb_vm_info.memory_usage)) {
        LOG4CPLUS_ERROR(logger, "Parse " << ATTR_VMHB_MEMORY << " error.");
        return;
    }
 
    if (!ad_ptr->EvaluateAttrNumber(ATTR_VMHB_BYTES_IN, m_hb_vm_info.bytes_in)) {
        LOG4CPLUS_ERROR(logger, "Parse " << ATTR_VMHB_BYTES_IN << " error.");
        return;
    }

    if (!ad_ptr->EvaluateAttrNumber(ATTR_VMHB_BYTES_OUT, m_hb_vm_info.bytes_out)) {
        LOG4CPLUS_ERROR(logger, "Parse " << ATTR_VMHB_BYTES_OUT << " error.");
        return;
    }
 
    int32_t app_state;
    if (!ad_ptr->EvaluateAttrNumber(ATTR_VMHB_STATE, app_state)) {
        LOG4CPLUS_ERROR(logger, "Parse " << ATTR_VMHB_STATE << " error.");
        return;
    }

    TaskID id = GetID();
    if(app_state == 2) {
	if(m_hb_vm_info.app_state == AppState::APP_RUNNING) {
            LOG4CPLUS_INFO(logger, "Handle event of taskrunning state, job_id:" << id.job_id << ", task_id:" << id.task_id);
	}
	m_hb_vm_info.app_state = AppState::APP_RUNNING;
        std::cout<<"task state running"<<std::endl;
	// update task  state into running
        // new runningActionEvent
        EventPtr event(new RunningTaskEvent(id));
        // Push event into Queue
        EventDispatcherI::Instance()->Dispatch(event->GetType())->PushBack(event);
    } else if(app_state == 3) {
	m_hb_vm_info.app_state = AppState::APP_FINISHED;
	VMPoolI::Instance()->SetVMStateByTaskID(id, false);
	// update task  state into finished
        // new finishedActionEvent
        EventPtr event(new FinishedTaskEvent(id));
        // Push event into Queue
        EventDispatcherI::Instance()->Dispatch(event->GetType())->PushBack(event);
    } else if(app_state == 4) {
	m_hb_vm_info.app_state = AppState::APP_FAILED;
	VMPoolI::Instance()->SetVMStateByTaskID(id, false);
	// update task  state into failed
        // new FailedActionEvent
        EventPtr event(new FailedTaskEvent(id));
        // Push event into Queue
        EventDispatcherI::Instance()->Dispatch(event->GetType())->PushBack(event);
    } else {
        m_hb_vm_info.app_state = AppState::APP_FAILED;
	VMPoolI::Instance()->SetVMStateByTaskID(id, false);
	// update task  state into failed
        // new FailedActionEvent
        EventPtr event(new FailedTaskEvent(id));
        // Push event into Queue
        EventDispatcherI::Instance()->Dispatch(event->GetType())->PushBack(event);
    }
}

/// unique in KVM 
virDomainPtr KVM::GetDomainPtr() const {
    return m_domain_ptr;
}

string KVM::GetEndpoint() const {
    TaskInfo info = GetTaskInfo();
    stringstream ss;
    ss << GetTaskInfo().vm_info.port;
    return info.vm_info.ip + ":" + ss.str();
}

//void KVM::SetDomainPtr(virDomainPtr ptr) {
//    m_domain_ptr = ptr;
//}

int32_t KVM::GetVNCPort() const {
    return m_vnc_port;
}

//void KVM::SetVNCPort(int32_t port) {
//    m_vnc_port = port;
//}

string KVM::GetVNet() const {
    return m_vnet;
}

void KVM::SetVNet(string vnet) {
    m_vnet = vnet;
}


/// @brief: private function
// set name
void KVM::SetName() {
    // app_name + "_kvm_" + job_id + "_" + task_id
    TaskID id = GetID();
    stringstream ss_job, ss_task;
    ss_job << id.job_id;
    ss_task << id.task_id;
    string name = GetTaskInfo().app_info.name + "_kvm_" + ss_job.str() + "_" + ss_task.str();
    SetNameByString(name);
}

// init heartbeat
void KVM::InitHeartbeat() {
    m_hb_vm_info.id = GetID();
    m_hb_vm_info.name = GetName();
    m_hb_vm_info.type = GetVMType();
}

// set name heartbeat img iso, mk work dir, build libvirt connection, read xml_template into m_xml
int32_t KVM::Init() {
    // set name
    SetName();
    // init hb
    InitHeartbeat();

    // set dir, img, iso
    TaskID id = GetID();
    stringstream ss_job, ss_task;
    ss_job << id.job_id;
    ss_task << id.task_id;
    m_dir = FLAGS_libvirt_dir + "/" + GetName() + "/";
    m_img = m_dir + "kvm_" + ss_job.str() + "_" + ss_task.str() + ".img";
    m_iso = m_dir + "kvm_" + ss_job.str() + "_" + ss_task.str() + ".iso";
    m_conf = m_dir + "CONF";

     
    // check total libvirt work directory
    if (chdir(FLAGS_libvirt_dir.c_str()) < 0) {
        LOG4CPLUS_ERROR(logger, "No libvirt work directory:" << FLAGS_libvirt_dir);
        return -1;
    }
    // mkdir work dir
    if (access(m_dir.c_str(), F_OK) == -1) {
        if (mkdir(m_dir.c_str(), 0755) != 0) {
           LOG4CPLUS_ERROR(logger, "Failed to create kvm work dir:" << m_dir);
           return -1;
        }
    }

    // build connection
    if (NULL == m_conn) {
        m_conn = virConnectOpen("qemu:///system");
        if(NULL == m_conn) {
            LOG4CPLUS_ERROR(logger, "Failed to open connection to qemu:///system");
            return -1;
        }
    }

    // read xml template into m_xml_template
    if ("" == m_xml_template) {
        // open libvirt xml template
        ifstream file(FLAGS_xml_template.c_str());
        if (!file) {
            LOG4CPLUS_ERROR(logger, "Failed to read xml template file: " << FLAGS_xml_template);
            return -1;
        }

        // read xml template content into m_xml
        stringstream ss;
        ss << file.rdbuf();
        m_xml_template = ss.str();
    }

    // set m_xml
    m_xml = m_xml_template;

    return 0;
}

// cp img, type = raw
int32_t KVM::CopyImage() {
    // img is exist?
    string img_template = FLAGS_libvirt_dir + GetTaskInfo().vm_info.img_template;
    if (access(img_template.c_str(), F_OK) == -1) {
        LOG4CPLUS_ERROR(logger, "Template " << img_template << " dose not exits");
        return -1;
    }

    // cp img
    string cmd = "cp " + img_template + " " + m_img;
    int32_t ret = system(cmd.c_str());
    ret = ret >> 8;
    if (ret != 0) {
        LOG4CPLUS_ERROR(logger, "Failed to copy image template");
        return -1;
    }
    return 0;
}

// clone img from img template, type = qcow2 .qco
int32_t KVM::CloneImage() {
     //check image by conf
    string user = GetTaskInfo().user;
    string image_template = GetTaskInfo().vm_info.img_template;
    int32_t image_size = GetTaskInfo().vm_info.size;
    stringstream image_size_ss;
    image_size_ss << image_size; 
    string image_attr = user + "&" + image_template + "&" + image_size_ss.str();
    if(false == ImageMgrI::Instance()->CheckImageAttrFromFile(image_attr)){
        LOG4CPLUS_ERROR(logger, "Template " << image_template << " dose not exits");
        return -1;
    }   

    // img is exist?
    /*
    string img_template = FLAGS_libvirt_dir + GetTaskInfo().vm_info.img_template;
    if (access(img_template.c_str(), F_OK) == -1) {
        LOG4CPLUS_ERROR(logger, "Template " << img_template << " dose not exits");
        return -1;
    }
    */
    // cp img
    string img_template = FLAGS_libvirt_dir + GetTaskInfo().vm_info.img_template;
    string cmd = "qemu-img create -b " + img_template + " -f qcow2 " + m_img + " > /dev/null 2>&1";
    int32_t ret = system(cmd.c_str());
    ret = ret >> 8;
    if (ret != 0) {
        LOG4CPLUS_ERROR(logger, "Failed to clone image template");
        return -1;
    }
    return 0;
 
}

// customize libvirt xml config file according @vm_info
int32_t KVM::ConfigVirXML() {
    // read xml content into xml_conf
    string xml_conf = m_xml;

    // customize xml
    // name
    string::size_type pos = xml_conf.find("T_NAME");
    if (pos == string::npos) {
        LOG4CPLUS_ERROR(logger, "Error in finding T_NAME in kvm xml template");
        return -1;
    }
    xml_conf.replace(pos, strlen("T_NAME"), GetName());

    // memory
    pos = xml_conf.find("T_MEMORY");
    if (pos == string::npos) {
        LOG4CPLUS_ERROR(logger, "Error in finding T_MEMORY in kvm xml template");
        return -1;
    }

    stringstream ss;
    ss.str("");
    ss.clear();
    ss << (GetTaskInfo().vm_info.memory * 1024);
    xml_conf.replace(pos, strlen("T_MEMORY"), ss.str());
  
    // vcpu 
    pos = xml_conf.find("T_VCPU");
    if (pos == string::npos) {
        LOG4CPLUS_ERROR(logger, "Error in finding T_VCPU in kvm xml template");
        return -1;
    }
    ss.str("");
    ss.clear();
    //vcpu以及cpu_shares目前是有联系的，对于kvm来说，CPU最好是个整数
    ss << GetTaskInfo().vm_info.vcpu;
    xml_conf.replace(pos, strlen("T_VCPU"), ss.str());

    /* // CPU_SHARE
    pos = xml_copy.find("T_CPU_SHARE");
    if (pos == string::npos) {
        LOG4CPLUS_ERROR(logger, "error in finding T_CPU_SHARE in vm template");
        return -1;
    }
    ss.str("");
    ss.clear();
    // cpu 参数设置的是cpu的share值
    // 一个cpu对应的归一化参数是1024 
    ss << GetTaskInfo().vm_info.vcpu * 1024;
    xml_conf.replace(pos, strlen("T_CPU_SHARE"), ss.str());
    */

    // pos = xml_copy.find("T_OUT_BOUND");
    // pos = xml_copy.find("T_IN_BOUND");

    // boot
    pos =  xml_conf.find("T_BOOT");
    if (pos == string::npos) {
        LOG4CPLUS_ERROR(logger, "Error in finding T_BOOT in kvm xml template");
        return -1;
    }
    if (!GetTaskInfo().vm_info.iso_location.empty()) {
        // start from iso 
        xml_conf.replace(pos, strlen("T_BOOT"), "cdrom");
    } else {
        // start from hard disk
        xml_conf.replace(pos, strlen("T_BOOT"), "hd");
    }

    // img
    pos = xml_conf.find("T_IMG_LOCATION");
    if (pos == string::npos) {
        LOG4CPLUS_ERROR(logger, "Error in finding T_IMG_LOCATION in kvm xml template");
        return -1;
    }
    xml_conf.replace(pos, strlen("T_IMG_LOCATION"), m_img);

    // iso
    pos = xml_conf.find("T_ISO_LOCATION");
    if (pos == string::npos) {
        LOG4CPLUS_ERROR(logger, "Error in finding T_ISO_LOCATION in kvm xml template");
        return -1;
    }
    xml_conf.replace(pos, strlen("T_ISO_LOCATION"), m_iso);

    /*
    // vnc port
    pos =  xml_conf.find("T_VNC_PORT");
    if (pos == string::npos) {
        LOG4CPLUS_ERROR(logger, "Error in finding T_VNC_PORT in kvm xml template");
        return -1;
    }
    ss.str("");
    ss.clear();
    ss << GetTaskInfo().vm_info.vnc_port + 5900;
    xml_conf.replace(pos, strlen("T_VNC_PORT"), ss.str());
    */
    m_xml = xml_conf; 

    return 0;
}

// create libvirt kvm
int32_t KVM::CreateKVM() {
    // task is exist?
    TaskID id = GetID();
    TaskPtr task_ptr = GetTaskPtr();
    // TODO 
    // TaskPtr task_ptr = TaskPoolI::Instance()->GetTaskPtr(task_id);
    // can't find the task, taskptr = NULL
    if (!task_ptr) {
        LOG4CPLUS_ERROR(logger, "Failed to find task, job_id:" << id.job_id << ", task_id" << id.task_id);
        task_ptr->TaskFailed();
        return -1;
    }

    // init, set name img iso, mk work dir, cp libvirt xml
    if (Init() != 0) {
        LOG4CPLUS_ERROR(logger, "Failed to initialize kvm");
        task_ptr->TaskFailed();        
        return -1;
    }
    task_ptr->SetStates(TaskEntityState::TASKENTITY_STARTING, 5.0);

    // clone img
    if (CloneImage() != 0) {
        LOG4CPLUS_ERROR(logger, "Failed to clone image");
        task_ptr->TaskFailed();
        return -1;
    }
    task_ptr->SetStates(TaskEntityState::TASKENTITY_STARTING, 25.0);

    // config iso, include ip, app
    string user = GetTaskInfo().user;
    string name = GetTaskInfo().app_info.name;
    string app_file = GetTaskInfo().app_info.app_file;
    string app_result_path = "/lynn/" + user + "/app/" + name + "/result/";
    string app_src_path = "/lynn/" + user + "/app/" + name + "/" + app_file;
    stringstream port_ss;
    port_ss << FLAGS_hdfs_port;
    string hdfs_endpoint = FLAGS_hdfs_host + ":" + port_ss.str();
    ofstream conf_file(m_conf.c_str());
    conf_file << "[vm_worker]" << endl;
    conf_file << "job_id = " << GetID().job_id << endl;
    conf_file << "task_id = " << GetID().task_id << endl;
    conf_file << "name = " << GetName() << endl;
    //conf_file << "os = " << GetTaskInfo().vm_info.os << endl;
    conf_file << "app_name = " << GetTaskInfo().app_info.name << endl;
    conf_file << "app_result_path = " << app_result_path << endl;
    conf_file << "app_src_path = " << app_src_path << endl;
    conf_file << "exe = " << GetTaskInfo().app_info.exe << endl;
    conf_file << "rpc_running = " << GetTaskInfo().vm_info.rpc_running << endl;
    conf_file << "interface = " << "eth0" << endl;
    conf_file << "ip = " << GetTaskInfo().vm_info.ip << endl;
    conf_file << "port = " << GetTaskInfo().vm_info.port << endl;
    conf_file << "executor_endpoint = " 
              << ResourceMgrI::Instance()->GetBridgeEndpoint() << endl;
    conf_file << "hdfs_endpoint = " << hdfs_endpoint << endl;
    conf_file << "heartbeat_interval = " << FLAGS_vm_hb_interval << endl;
    conf_file.close();
    
    string cmd = "mkisofs -o " + m_iso + " " + m_conf + " > /dev/null 2>&1";
    int32_t ret = system(cmd.c_str());
    ret = ret >> 8;
    if (ret != 0) {
        LOG4CPLUS_ERROR(logger, "Failed to create conf iso file");
        task_ptr->TaskFailed();
        return -1;
    }
    task_ptr->SetStates(TaskEntityState::TASKENTITY_STARTING, 30.0);

    // libvirt template XML
    if (ConfigVirXML() != 0) {
        LOG4CPLUS_ERROR(logger, "Failed to customize kvm xml");
        task_ptr->TaskFailed();
        return -1;
    }
    task_ptr->SetStates(TaskEntityState::TASKENTITY_STARTING, 35.0);
 
    // create vm
    m_domain_ptr = virDomainCreateXML(m_conn, m_xml.c_str(), 0);
    if (!m_domain_ptr) {
        virErrorPtr error = virGetLastError();
        LOG4CPLUS_ERROR(logger, "Failed to create kvm by libvirt xml," << error->message);
        task_ptr->TaskFailed();
        return -1;
    }
    task_ptr->SetStates(TaskEntityState::TASKENTITY_STARTING, 40.0);
   
    return 0;    
}

// after create vm
int32_t KVM::SetVNetByXML() {
    // get XML by vir_ptr
    char *xml = virDomainGetXMLDesc(m_domain_ptr, 0);

    // parse xml doc
    xml_document<> doc;
    try {
        doc.parse<0>(xml);
        // FATAL: free xml at the end
    } catch (rapidxml::parse_error& ex) {
        LOG4CPLUS_ERROR(logger, "synex error in " << ex.what());
        return -1;
    } catch (std::runtime_error& ex) {
        LOG4CPLUS_ERROR(logger, "vm xml error:" << ex.what());
        return -1;
    }

    // parse, get vnet from xml
    xml_node<> *root = doc.first_node("domain");
    if (!root) {
        LOG4CPLUS_ERROR(logger, "no domain found.");
        return -1;
    }

    xml_node<> *node_level1, *node_level2, *node_level3;
    xml_attribute<> *attr;

    node_level1 = root->first_node("devices");
    if (!node_level1) {
        LOG4CPLUS_ERROR(logger, "no devices found.");
        return -1;
    }

    node_level2 = node_level1->first_node("interface");
    if (!node_level2) {
        LOG4CPLUS_ERROR(logger, "no interface found.");
        return -1;
    }

    node_level3 = node_level2->first_node("target");
    if (!node_level3) {
        LOG4CPLUS_ERROR(logger, "no target found.");
        return -1;
    }

    attr = node_level3->first_attribute("dev");
    if (!attr) {
        LOG4CPLUS_ERROR(logger, "no interface dev found.");
        return -1;
    }

    // set vnet
    SetVNet(attr->value());
    free(xml);
    return 0;
}
