/********************************
 FileName: JobsManager/job_parser.cpp
 Author:   ZhangZhang & WangMin
 Date:     2014-04-08
 Version:  0.1
 Description: parse job from xml file, init job and tasks
*********************************/

#include <classad/classad.h>
#include <classad/classad_distribution.h>

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

#include "common/rapidxml/rapidxml.hpp"
#include "common/rapidxml/rapidxml_utils.hpp"
#include "common/rapidxml/rapidxml_print.hpp"
#include "common/clynn/string_utility.h"

#include "iostream"
#include "include/classad_attr.h"
#include "JobsManager/jobs_manager_service.h"
#include "JobsManager/job_enum.h"
#include "JobsManager/job_parser.h"
#include "JobsManager/job_pool.h"
#include "JobsManager/group_pool.h"
#include "JobsManager/batch_job.h"

using log4cplus::Logger;
using namespace rapidxml;
using std::cout;

static Logger logger = Logger::getInstance("JobsManager");

#define MAX_WEIGHT 100
#define MAX_WEIGHT_N -100

JobParser::JobParser() {
    m_job_classad_ptr = ClassAdPtr(new ClassAd());
    m_job_id = 0;
    m_errno = 0;
}

JobParser::~JobParser() {
   //清空已生成的task classad
   m_taskad_list.clear();
}

int32_t JobParser::SubmitNewJob(const string& job_xml, int32_t job_id) {
    LOG4CPLUS_INFO(logger, "submitting a new job:" << std::endl << job_xml);

    if (!ParseJobAd(job_xml)) {
        LOG4CPLUS_ERROR(logger, "Failed to submit a new job:" << job_xml);
        return m_errno;
    }

    JobPtr job_ptr = CreateJobPtr();
    if (NULL == job_ptr) {
        return m_errno;
    }

    // test TODO 
    job_ptr->PrintAllTasks();

    JobPoolI::Instance()->InsertIfAbsent(job_ptr);
    // test TODO
    JobPoolI::Instance()->PrintAll();
    NewJobList::Instance()->PushBack(job_ptr);
    return 0;
}

void JobParser::PrintAllTasks() {
    printf("-------- tasks --------\n");
    string str_classad = ClassAdComplement::AdTostring(m_job_classad_ptr);
    printf("job classad: %s\n", str_classad.c_str());

    for (list<TaskAdPtr>::iterator it = m_taskad_list.begin();
         it != m_taskad_list.end(); ++it)
    {
         // hard constraint
         ClassAdPtr hard_ptr = (*it)->taskad_hard_constraint;
         string str_hard_classad = ClassAdComplement::AdTostring(hard_ptr);
         printf("task hard classad: %s\n", str_hard_classad.c_str());
         // soft constraint list
         for (list<ClassAdPtr>::iterator inner_it = (*it)->soft_constraint_list.begin();
              inner_it != (*it)->soft_constraint_list.end(); ++ inner_it)
         {
             ClassAdPtr soft_ptr = *inner_it;
             string str_soft_classad = ClassAdComplement::AdTostring(soft_ptr);
             printf("task soft classad: %s\n", str_soft_classad.c_str());
         }
    }

    // constraints among tasks
    for (list<EdgePtr>::iterator it = m_edge_list.begin();
         it != m_edge_list.end(); ++it)
    {
        printf("start: %d, end: %d, weight: %d\n", (*it)->GetStart(), (*it)->GetEnd(), (*it)->GetWeight());
    }
    printf("---------------------\n");
}

// parse job ClassAd from xml content
bool JobParser::ParseJobAd(const string& job_xml) {   
    string::size_type position = job_xml.find("<xml_type>heterogeneous</xml_type>");
    if (position != job_xml.npos) 
        return ParseJobAdHe(job_xml);
    else
        return ParseJobAdHo(job_xml);

    return false;
} 


// parse job ClassAd from xml content, heterogenous(yi gou)
bool JobParser::ParseJobAdHe(const string& job_xml) {
    // printf("heterogenous\n");
    xml_document<> doc;
    try {
        doc.parse<0>(const_cast<char *>(job_xml.c_str()));
    } catch (parse_error& ex) {
        LOG4CPLUS_ERROR(logger, "synex error in " << ex.what() << ".");
        return false;
    } catch (std::runtime_error& ex) {
        LOG4CPLUS_ERROR(logger, "xml error:" << ex.what() << ".");
        return false;
    }

    // xml_root job
    xml_node<> *root_node = doc.first_node("job");
    if (!root_node) {
        LOG4CPLUS_ERROR(logger, "no job specified.");
        return false;
    }

    // init m_job_classad_ptr
    // xml node, group, priority, job_type
    xml_node<> *node; 
    // group
    node = root_node->first_node("group");
    if (!node) {
        LOG4CPLUS_ERROR(logger, "No group specified.");
        return false;
    }
    string group = node->value();
    if (!GroupPoolI::Instance()->IsExistByName(group)) {
        LOG4CPLUS_ERROR(logger, "No this group: " << group);
    }
    m_job_classad_ptr->InsertAttr(ATTR_GROUP, group);

    // priority   
    node = root_node->first_node("priority");
    if (!node) {
        LOG4CPLUS_ERROR(logger, "no priority specified.");
        return false;
    }
    string str_job_prio = node->value();
    JobRawPrio job_prio;
    if (!GetJobPrio(str_job_prio, job_prio)) {
        LOG4CPLUS_ERROR(logger, "no this job_prio: " << str_job_prio);
        return false;
    }
    m_job_classad_ptr->InsertAttr(ATTR_PRIO, job_prio);

    // job_type
    node = root_node->first_node("job_type");
    if (!node) {
        LOG4CPLUS_ERROR(logger, "no job_type specified.");
        return false;
    }
    string str_job_type = node->value();
    JobType job_type;
    if (!GetJobType(str_job_type, job_type)) {
        LOG4CPLUS_ERROR(logger, "no this job_type: " << str_job_type);
        return false;
    }
    m_job_classad_ptr->InsertAttr(ATTR_JOB_TYPE, int32_t(job_type));

    
    // init list<TaskAdPtr> m_taskad_list;
    xml_node<> *tasks_node = root_node->first_node("tasks");
    if (!tasks_node) {
        LOG4CPLUS_ERROR(logger, "No tasks specified.");
        return false;
    }

    int32_t total_tasks_number = 0;
    double total_need_cpu = 0.0;
    int32_t total_need_memory = 0;

    // traversal(bian li) all tasks 
    for (xml_node<char> *task_node = tasks_node->first_node("task");
        task_node != NULL;
        task_node = task_node->next_sibling())
    {
        // parse xml, node of task
        xml_node<char> *t_node;

        // task_ids
        t_node = task_node->first_node("task_id");
        if (!t_node) {
            LOG4CPLUS_ERROR(logger, "No task_ids specified");
            return false;
        }
        string task_id = t_node->value();

        // split
        vector<string> ids;
        string separator = ",";
        StringUtility::Split(task_id, ids, separator);

        // tmp TaskAdPtr
        TaskAdPtr tmpTaskAdPtr = TaskAdPtr(new TaskAd());
        (tmpTaskAdPtr->taskad_hard_constraint)->InsertAttr(ATTR_GROUP, group);
        (tmpTaskAdPtr->taskad_hard_constraint)->InsertAttr(ATTR_PRIO, job_prio);

        // need_cpu
        t_node = task_node->first_node("need_cpu");
        if (!t_node) {
            LOG4CPLUS_ERROR(logger, "No need_cpu specified");
            return false;
        }
        double need_cpu = atof(t_node->value());
        if (need_cpu <= 0) {
            LOG4CPLUS_ERROR(logger, "Need_cpu is illegal.");
            return false;
        }
        (tmpTaskAdPtr->taskad_hard_constraint)->InsertAttr(ATTR_NEED_CPU, need_cpu);

        // need_memory
        t_node = task_node->first_node("need_memory");
        if (!t_node) {
            LOG4CPLUS_ERROR(logger, "No need_memory specified");
            return false;
        }
        double need_memory = atof(t_node->value());
        if (need_memory <= 0) {
            LOG4CPLUS_ERROR(logger, "Need_memory is illegal.");
            return false;
        }
        (tmpTaskAdPtr->taskad_hard_constraint)->InsertAttr(ATTR_NEED_MEMORY, need_memory);
        
        // img_template
        // is not necessary
        t_node = task_node->first_node("img_template");
        if (t_node) {
            // LOG4CPLUS_INFO(logger, "img_template specified");
            string img_template = t_node->value();
            (tmpTaskAdPtr->taskad_hard_constraint)->InsertAttr(ATTR_IMG, img_template); 
        }

        // exe_path
        t_node = task_node->first_node("exe_path");
        if (!t_node) {
            LOG4CPLUS_ERROR(logger, "No exe_path specified");
            return false;
        }
        string exe_path = t_node->value();
        (tmpTaskAdPtr->taskad_hard_constraint)->InsertAttr(ATTR_EXE_PATH, exe_path); 

        // hard requirement
        t_node = task_node->first_node("hard_requirement");
        if (t_node) {
            // LOG4CPLUS_INFO(logger, "hard_requirement specified");
            string hard_requirement = t_node->value();
            (tmpTaskAdPtr->taskad_hard_constraint)->InsertAttr(ATTR_HARD_REQUIREMENT, hard_requirement);
        }

        for (t_node = task_node->first_node("soft_requirement");
             t_node != NULL;
             t_node = t_node->next_sibling())
        {
            ClassAdPtr soft_ptr = ClassAdPtr(new ClassAd());
            string soft_requirement = t_node->value();
            ClassAdParser parser;
            ExprTree* soft_expr = parser.ParseExpression(soft_requirement);
            soft_ptr->Insert(ATTR_SOFT_REQUIREMENT, soft_expr);
            xml_attribute<char> * attr = t_node->first_attribute("value");
            int32_t value = atoi(attr->value());
            soft_ptr->InsertAttr(ATTR_SOFT_VALUE, value);
            (tmpTaskAdPtr->soft_constraint_list).push_back(soft_ptr);
        } 

        // copy multi TaskAd according ids 
        vector<string>::iterator it = ids.begin();
        for (; it != ids.end(); ++it) {
            int32_t task_id = atoi((*it).c_str());
            if (task_id <= 0) { 
                LOG4CPLUS_ERROR(logger, "task_id is illegal.");
                return false;
            }
            if (FindId(task_id)) {
                LOG4CPLUS_ERROR(logger, "task_id is repeated.");
                return false;
            }
            InsertId(task_id);

            // total
            total_tasks_number++;
            total_need_cpu += need_cpu;
            total_need_memory += need_memory;

            // deep copy
            TaskAdPtr copy_ptr = TaskAdPtr(new TaskAd(*tmpTaskAdPtr));
            (copy_ptr->taskad_hard_constraint)->InsertAttr(ATTR_TASK_ID, task_id);
            m_taskad_list.push_back(copy_ptr);
            /* test: deep copy check
            if (it == ids.begin()) {
                if (!(tmpTaskAdPtr->soft_constraint_list).empty())
                    (tmpTaskAdPtr->soft_constraint_list).pop_front(); 
            } */ 
            // PrintAllTasks();
        }
    }

    m_job_classad_ptr->InsertAttr(ATTR_TOTAL_TASKS, total_tasks_number);
    m_job_classad_ptr->InsertAttr(ATTR_NEED_CPU, total_need_cpu);
    m_job_classad_ptr->InsertAttr(ATTR_NEED_MEMORY, total_need_memory);


    // constraints among tasks
    // init list<TaskAdPtr> m_taskad_list;
    xml_node<> *edges_node = root_node->first_node("constraints");
    if (!edges_node) {
        // No Error, can't return false
        LOG4CPLUS_INFO(logger, "No constraints specified.");
        return true;
    }

    // traversal(bian li) all edges 
    for (xml_node<char> *edge_node = edges_node->first_node("edge");
        edge_node != NULL;
        edge_node = edge_node->next_sibling())
    {
        // edge node, including start, end, weiht
        xml_node<> *e_node;
        // start
        e_node = edge_node->first_node("start"); 
        if (!e_node) {
            LOG4CPLUS_ERROR(logger, "No start specified");
            return false;
        }
        int32_t start = atoi(e_node->value());
        if (start < 0 || !FindId(start)) {
            LOG4CPLUS_ERROR(logger, "start is illegal.");
            return false;
        }

        // end
        e_node = edge_node->first_node("end");
        if (!e_node) {
            LOG4CPLUS_ERROR(logger, "No end specified");
            return false;
        }
        int32_t end = atoi(e_node->value());
        if (end < 0 || !FindId(end)) {
            LOG4CPLUS_ERROR(logger, "end is illegal.");
            return false;
        }

        // weight
        e_node = edge_node->first_node("weight");
        if (!e_node) {
            LOG4CPLUS_ERROR(logger, "No weight specified");
            return false;
        }
        int32_t weight = atoi(e_node->value());
        if (0 == weight) {
            LOG4CPLUS_ERROR(logger, "weight is illegal.");
            return false;
        }

        if (weight > MAX_WEIGHT) 
            weight = MAX_WEIGHT;
        if (weight < MAX_WEIGHT_N)
            weight = MAX_WEIGHT_N;
        
        // insert edge
        EdgePtr edge_ptr = EdgePtr(new ConstraintEdge(start, end, weight)); 
        m_edge_list.push_back(edge_ptr);
    } 

    m_task_ids.clear();
    return true; 
}

// parse job ClassAd from xml content, homogenous(tong gou)
bool JobParser::ParseJobAdHo(const string& job_xml) {
    printf("homogenous\n");
    xml_document<> doc;
    try {
        doc.parse<0>(const_cast<char *>(job_xml.c_str()));
    } catch (parse_error& ex) {
        LOG4CPLUS_ERROR(logger, "synex error in " << ex.what() << ".");
        return false;
    } catch (std::runtime_error& ex) {
        LOG4CPLUS_ERROR(logger, "xml error:" << ex.what() << ".");
        return false;
    }

    // xml_root job
    xml_node<> *root_node = doc.first_node("job");
    if (!root_node) {
        LOG4CPLUS_ERROR(logger, "no job specified.");
        return false;
    }

    // init m_job_classad_ptr
    // xml node, group, priority, job_type, ...
    xml_node<> *node;
    // group
    node = root_node->first_node("group");
    if (!node) {
        LOG4CPLUS_ERROR(logger, "No group specified.");
        return false;
    }
    string group = node->value();
    if (!GroupPoolI::Instance()->IsExistByName(group)) {
        LOG4CPLUS_ERROR(logger, "No this group: " << group);
    }
    m_job_classad_ptr->InsertAttr(ATTR_GROUP, group);
    m_job_classad_ptr->InsertAttr(ATTR_USER, group);


    // priority   
    node = root_node->first_node("priority");
    if (!node) {
        LOG4CPLUS_ERROR(logger, "no priority specified.");
        return false;
    }
    string str_job_prio = node->value();
    JobRawPrio job_prio;
    if (!GetJobPrio(str_job_prio, job_prio)) {
        LOG4CPLUS_ERROR(logger, "no this job_prio: " << str_job_prio);
        return false;
    }
    m_job_classad_ptr->InsertAttr(ATTR_PRIO, job_prio);

    // job_type
    node = root_node->first_node("job_type");
    if (!node) {
        LOG4CPLUS_ERROR(logger, "no job_type specified.");
        return false;
    }
    string str_job_type = node->value();
    JobType job_type;
    if (!GetJobType(str_job_type, job_type)) {
        LOG4CPLUS_ERROR(logger, "no this job_type: " << str_job_type);
        return false;
    }
    m_job_classad_ptr->InsertAttr(ATTR_JOB_TYPE, int32_t(job_type));

    // sched model
    node = root_node->first_node("sched_model");
    SchedModel sched_model;
    if (!node) {
        LOG4CPLUS_ERROR(logger, "No sched_model specified.");
        m_job_classad_ptr->InsertAttr(ATTR_SCHED_MODEL, DEFAULT_MODEL);
    } else {
        string str_sched_model = node->value();
        if (!GetSchedModel(str_sched_model, sched_model)) {
            LOG4CPLUS_ERROR(logger, "no this model: " << str_sched_model);
            return false;
        }
        m_job_classad_ptr->InsertAttr(ATTR_SCHED_MODEL, int32_t(sched_model));
    }

    // need_cpu
    node = root_node->first_node("need_cpu");
    if (!node) {
        LOG4CPLUS_ERROR(logger, "No need_cpu specified");
        return false;
    }
    double need_cpu = atof(node->value());
    if (need_cpu <= 0) {
        LOG4CPLUS_ERROR(logger, "Need_cpu is illegal.");
        return false;
    }
    m_job_classad_ptr->InsertAttr(ATTR_NEED_CPU, need_cpu);

    // need_memory
    node = root_node->first_node("need_memory");
    if (!node) {
        LOG4CPLUS_ERROR(logger, "No need_memory specified");
        return false;
    }
    double need_memory = atof(node->value());
    if (need_memory <= 0) {
        LOG4CPLUS_ERROR(logger, "Need_memory is illegal.");
        return false;
    }
    m_job_classad_ptr->InsertAttr(ATTR_NEED_MEMORY, need_memory);

    // vm_type
    node = root_node->first_node("vm_type");
    if (!node) {
        LOG4CPLUS_ERROR(logger, "No vm_type specified");
        return false;
    }
    string vm_type = node->value();
    int32_t vm_t = 0;
    if ("LXC" == vm_type) {
        vm_t = 2;
    } else {
        vm_t = 1;
    }
    m_job_classad_ptr->InsertAttr(ATTR_VMTYPE, vm_t);
 
    // img_template
    // TODO is not necessary for lxc
    node = root_node->first_node("img_template");
    if (!node) {
        LOG4CPLUS_ERROR(logger, "No img_template specified");
        return false;
    }
    string img_template = node->value();
    m_job_classad_ptr->InsertAttr(ATTR_Image, img_template);

    // size
    node = root_node->first_node("size");
    if (!node) {
        LOG4CPLUS_ERROR(logger, "No size specified");
        return false;
    }
    double size = atoi(node->value());
    if (size <= 0) {
        LOG4CPLUS_ERROR(logger, "size is illegal.");
        return false;
    }
    m_job_classad_ptr->InsertAttr(ATTR_SIZE, size);

    // app name
    node = root_node->first_node("app_name");
    if (!node) {
        LOG4CPLUS_ERROR(logger, "No app_name specified");
        return false;
    }
    string app_name = node->value();
    m_job_classad_ptr->InsertAttr(ATTR_APP_NAME, app_name);

    // app file 
    node = root_node->first_node("app_file");
    if (!node) {
        LOG4CPLUS_ERROR(logger, "No app_file specified");
        return false;
    }
    string app_file = node->value();
    m_job_classad_ptr->InsertAttr(ATTR_APP_NAME, app_file);


    // timeout
    node = root_node->first_node("timeout");
    if (!node) {
        LOG4CPLUS_ERROR(logger, "timeout specified");
        return false;
    }
    int32_t timeout = atoi(node->value());
    if (timeout <= 0) {
        LOG4CPLUS_ERROR(logger, "timeout is illegal.");
        return false;
    }
    m_job_classad_ptr->InsertAttr(ATTR_TIMEOUT, timeout);

    // init list<TaskAdPtr> m_taskad_list;
    xml_node<> *tasks_node = root_node->first_node("tasks");
    if (!tasks_node) {
        LOG4CPLUS_ERROR(logger, "No tasks specified.");
        return false;
    }

    int32_t total_tasks_number = 0;
    int32_t task_id = 0;
    // traversal(bian li) all tasks 
    for (xml_node<char> *task_node = tasks_node->first_node("task");
        task_node != NULL;
        task_node = task_node->next_sibling())
    {
        // parse xml, node of task
        xml_node<char> *t_node;

        // tmp TaskAdPtr
        TaskAdPtr tmpTaskAdPtr = TaskAdPtr(new TaskAd());
        (tmpTaskAdPtr->taskad_hard_constraint)->InsertAttr(ATTR_GROUP, group);
        (tmpTaskAdPtr->taskad_hard_constraint)->InsertAttr(ATTR_USER, group);
        (tmpTaskAdPtr->taskad_hard_constraint)->InsertAttr(ATTR_PRIO, job_prio);
        (tmpTaskAdPtr->taskad_hard_constraint)->InsertAttr(ATTR_JOB_TYPE, job_type);
        (tmpTaskAdPtr->taskad_hard_constraint)->InsertAttr(ATTR_SCHED_MODEL, sched_model);
        (tmpTaskAdPtr->taskad_hard_constraint)->InsertAttr(ATTR_NEED_CPU, need_cpu);
        (tmpTaskAdPtr->taskad_hard_constraint)->InsertAttr(ATTR_NEED_MEMORY, need_memory);
        (tmpTaskAdPtr->taskad_hard_constraint)->InsertAttr(ATTR_VCPU, need_cpu);
        (tmpTaskAdPtr->taskad_hard_constraint)->InsertAttr(ATTR_MEMORY, need_memory);
        (tmpTaskAdPtr->taskad_hard_constraint)->InsertAttr(ATTR_VMTYPE, vm_t);
        (tmpTaskAdPtr->taskad_hard_constraint)->InsertAttr(ATTR_Image, img_template);
        (tmpTaskAdPtr->taskad_hard_constraint)->InsertAttr(ATTR_SIZE, size);
        (tmpTaskAdPtr->taskad_hard_constraint)->InsertAttr(ATTR_APP_NAME, app_name);
        (tmpTaskAdPtr->taskad_hard_constraint)->InsertAttr(ATTR_APP_FILE, app_file); 
        (tmpTaskAdPtr->taskad_hard_constraint)->InsertAttr(ATTR_TIMEOUT, timeout);
        // others default
        (tmpTaskAdPtr->taskad_hard_constraint)->InsertAttr(ATTR_IS_RUN, true);
        (tmpTaskAdPtr->taskad_hard_constraint)->InsertAttr(ATTR_PORT, 9991);
        (tmpTaskAdPtr->taskad_hard_constraint)->InsertAttr(ATTR_VNC_PORT, -1);
        (tmpTaskAdPtr->taskad_hard_constraint)->InsertAttr(ATTR_RPC_RUNNING, false);

        // exe_path
        t_node = task_node->first_node("exe_path");
        if (!t_node) {
            LOG4CPLUS_ERROR(logger, "No exe_path specified");
            return false;
        }
        string exe_path = t_node->value();
        (tmpTaskAdPtr->taskad_hard_constraint)->InsertAttr(ATTR_EXE_PATH, exe_path);

        // task num
        t_node = task_node->first_node("task_num");
        if (!t_node) {
            LOG4CPLUS_ERROR(logger, "No task_num specified");
            return false;
        }
        int32_t task_num = atoi(t_node->value());
        if (task_num <= 0) {
            LOG4CPLUS_ERROR(logger, "task_num is illegal.");
            return false;
        }

        // copy multi TaskAd according task_num 
        for (int32_t i = 0; i < task_num; ++i)  {
            total_tasks_number++;
            InsertId(task_id);
            // deep copy
            TaskAdPtr copy_ptr = TaskAdPtr(new TaskAd(*tmpTaskAdPtr));
            (copy_ptr->taskad_hard_constraint)->InsertAttr(ATTR_TASK_ID, task_id++);
            m_taskad_list.push_back(copy_ptr);
        }
    }
 
    // total need cpu & memory
    m_job_classad_ptr->InsertAttr(ATTR_TOTAL_TASKS, total_tasks_number);
    double total_need_cpu = need_cpu * total_tasks_number;
    int32_t total_need_memory = need_memory * total_tasks_number;
    m_job_classad_ptr->InsertAttr(ATTR_NEED_CPU, total_need_cpu);
    m_job_classad_ptr->InsertAttr(ATTR_NEED_MEMORY, total_need_memory);


    // constraints among tasks
    // init list<TaskAdPtr> m_taskad_list;
    xml_node<> *edges_node = root_node->first_node("constraints");
    if (!edges_node) {
        // No Error, can't return false
        LOG4CPLUS_INFO(logger, "No constraints specified.");
        return true;
    }

    // traversal(bian li) all edges 
    for (xml_node<char> *edge_node = edges_node->first_node("edge");
        edge_node != NULL;
        edge_node = edge_node->next_sibling())
    {
        // edge node, including start, end, weiht
        xml_node<> *e_node;
        // start
        e_node = edge_node->first_node("start");
        if (!e_node) {
            LOG4CPLUS_ERROR(logger, "No start specified");
            return false;
        }
        int32_t start = atoi(e_node->value());
        if (start < 0 || !FindId(start)) {
            LOG4CPLUS_ERROR(logger, "start is illegal, start: " << start);
            return false;
        }

        // end
        e_node = edge_node->first_node("end");
        if (!e_node) {
            LOG4CPLUS_ERROR(logger, "No end specified");
            return false;
        }
        int32_t end = atoi(e_node->value());
        if (end < 0 || !FindId(end)) {
            LOG4CPLUS_ERROR(logger, "end is illegal, end: " << end);
            return false;
        }

        // weight
        e_node = edge_node->first_node("weight");
        if (!e_node) {
            LOG4CPLUS_ERROR(logger, "No weight specified");
            return false;
        }
        int32_t weight = atoi(e_node->value());
        if (0 == weight) {
            LOG4CPLUS_ERROR(logger, "weight is illegal, weight: " << weight);
            return false;
        }

        if (weight > MAX_WEIGHT)
            weight = MAX_WEIGHT;
        if (weight < MAX_WEIGHT_N)
            weight = MAX_WEIGHT_N;

        // insert edge
        EdgePtr edge_ptr = EdgePtr(new ConstraintEdge(start, end, weight));
        m_edge_list.push_back(edge_ptr);
    }

    return true; 
}

JobPtr JobParser::CreateJobPtr() {
    if (m_taskad_list.empty()) {
        m_errno = JobError::E_JOB_NO_CLASSAD;
        return JobPtr();
    }
    int32_t i_job_type;
    m_job_classad_ptr->EvaluateAttrNumber(ATTR_JOB_TYPE, i_job_type);
    JobType job_type = JobType(i_job_type);

    // TODO 改为JobFactory类
    // 目前仅支持Batch Job
    JobPtr job_ptr;
    switch (job_type) {
        case JOB_TYPE_BATCH:
            job_ptr = JobPtr(new BatchJob());
            break;
        //缺省
        default:
            job_ptr = JobPtr(new BatchJob());
            break;
    }
    if (ConstraintsAmongTasks()) {
        job_ptr->Init(m_job_classad_ptr, true);
        job_ptr->InitTasks(m_taskad_list, m_edge_list);
    } else {
        job_ptr->Init(m_job_classad_ptr);
        job_ptr->InitTasks(m_taskad_list);
    }

    return job_ptr;
}

bool JobParser::GetJobType(const string& str_job_type, JobType& job_type) {
    if (str_job_type.empty()) {
        return false;
    }
   
    if ("batch" == str_job_type) {
        job_type = JOB_TYPE_BATCH;
        return true;
    } else if ("service" == str_job_type) {
        job_type = JOB_TYPE_SERVICE;
        return true;
    } else if ("cron" == str_job_type) {
        job_type = JOB_TYPE_CRON;
        return true;
    } else {
        job_type = JOB_TYPE_BATCH;         
    }

    return false; // return true;
}

bool JobParser::GetSchedModel(const string& str_sched_model, SchedModel& sched_model) {
    if (str_sched_model.empty()) {
        return false;
    }
  
    if ("DefaultModel" == str_sched_model || "Default" == str_sched_model) {
        sched_model = DEFAULT_MODEL;
        return true;
    } else if ("WideDistribution" == str_sched_model) {
        sched_model = WIDE_DISTRIBUTION;
        return true;
    } else {
        sched_model = DEFAULT_MODEL;
    }

    return false; // return true;

}

bool JobParser::GetJobPrio(const string& str_job_prio, JobRawPrio& job_prio) {
    if (str_job_prio.empty()) {
        return false;
    }

    if ("privilege" == str_job_prio) {
        job_prio = JOB_RAW_PRIO_PRIVILEGE;
        return true;
    } else if ("high" == str_job_prio) {
        job_prio = JOB_RAW_PRIO_HIGH;
        return true;
    } else if ("ordinary" == str_job_prio) {
        job_prio = JOB_RAW_PRIO_ORDINARY;
        return true;
    } else if ("low" == str_job_prio) {
        job_prio = JOB_RAW_PRIO_LOW;
        return true;
    } else {
        job_prio = JOB_RAW_PRIO_ORDINARY;
    }
   
    return false; // return true;
}

void JobParser::InsertId(int32_t task_id) {
    m_task_ids.push_back(task_id); 
}

bool JobParser::FindId(int32_t task_id) {
    auto it = std::find(m_task_ids.begin(), m_task_ids.end(), task_id);
    if (it != m_task_ids.end()) 
        return true;   
    return false;
}

bool JobParser::ConstraintsAmongTasks() {
    return !(m_edge_list.empty());
}
