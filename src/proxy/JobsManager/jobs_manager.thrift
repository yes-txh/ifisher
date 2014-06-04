struct SubmitJobResult {
    1: i32 job_id,
    2: i32 error_num,
}

struct TaskStateInfo {
    1:i32 job_id,
    2:i32 task_id,
    3:i32 state,
    4:string machine_ip,
    5:i32 restart_times,
    6:i32 failed_times,
    7:i32 timeout_times,
    8:i32 missed_times,    
}

struct GroupStateInfo {
    1:string name,
    2:i32 quota_shares,
    3:double quota_pct,
    4:double maxlimit_pct,
    5:double used_cpu,
    6:double used_memory,
    7:double total_cpu,
    8:double total_memory,
}

service JobsManager {
   SubmitJobResult SubmitJob(1:string job_xml),
   list<i32> GetJobId(),
   list<TaskStateInfo> GetTaskStateInfo(1:i32 job_id),
   i32 GetWaitJobNumByGroupName(1:string group),
   GroupStateInfo GetGroupStateInfo(1:string group),

   bool TaskStarted(1:i32 job_id, 2:i32 task_id),
   bool TaskFinished(1:i32 job_id, 2:i32 task_id),
   bool TaskFailed(1:i32 job_id, 2:i32 task_id),
   bool TaskMissed(1:i32 job_id, 2:i32 task_id),
   bool TaskTimeout(1:i32 job_id, 2:i32 task_id),

   bool TaskStartError(1:i32 job_id, 2:i32 task_id, 3:i32 err_num), 
   bool UpdateImage(1:string user, 2:string name, 3:i32 size 4:bool is_update_all),
   bool CreateApp(1:string user, 2:string app_name),
}
