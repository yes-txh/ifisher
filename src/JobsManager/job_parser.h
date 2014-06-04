/********************************
 FileName: JobsManager/job_parser.h
 Author:   ZhangZhang & WangMin
 Date:     2014-04-08
 Version:  0.1
 Description: parse job from xml file, init job and tasks
*********************************/

#ifndef JOBS_MANAGER_JOB_PARSER_H
#define JOBS_MANAGER_JOB_PAESER_H

#include <string>
#include <list>
#include <algorithm>
#include "common/classad/classad_complement.h"
#include "JobsManager/job_enum.h"
#include "JobsManager/job.h"
#include "JobsManager/constraint_edge.h"

using std::list;
using std::string;
using std::vector;

class JobParser {
public:
    JobParser();
    ~JobParser();
    int32_t SubmitNewJob(const string& job_xml, int32_t job_id);
    // TODO test
    void PrintAllTasks();
    // int SubmitNewJobByAdString(const string& job_ad, int& job_id);

private:
    // xml解析 + 函数嵌套后，XML string本身会发生改变， 故改为外层函数
    bool ParseJobAd(const string& job_xml);
    bool ParseJobAdHo(const string& job_xml);
    bool ParseJobAdHe(const string& job_xml);
    // bool ParseJobAdByAdString(const string& job_ad);
    JobPtr CreateJobPtr();
    bool GetJobPrio(const string& str_job_prio, JobRawPrio& prio_type);
    bool GetJobType(const string& str_job_type, JobType& job_type);
    bool GetSchedModel(const string& str_sched_model, SchedModel& sched_model);
    void InsertId(int32_t task_id);
    bool FindId(int32_t task_id);
    bool ConstraintsAmongTasks();

private:
    ClassAdPtr m_job_classad_ptr; 
    list<TaskAdPtr> m_taskad_list; // struct TaskAdPtr(TaskAd) from task.h 
    vector<int32_t> m_task_ids;
    list<EdgePtr> m_edge_list;
 
    int32_t m_job_id;
    int32_t m_errno;
};

#endif
