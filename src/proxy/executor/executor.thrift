//include "../scheduler/scheduler.thrift"
//include "../collector/collector.thrift"

enum TaskEntityState {
    TASKENTITY_WAITING,
    TASKENTITY_STARTING,
    TASKENTITY_RUNNING,
    TASKENTITY_FINISHED,
    TASKENTITY_FAILED,
    TASKENTITY_MISSED,
}

enum VMType {
    VM_UNKNOWN,
    VM_KVM,
    VM_LXC,
}

enum AppState {
    APP_WAITING,
    APP_STARTING,
    APP_RUNNING,
    APP_FINISHED,
    APP_MISSED,
    APP_FAILED,
}

enum EventType {
    TASK_STATE_EVENT,
    TASK_ACTION_EVENT,
    IMAGE_EVENT,
    HEARTBEAT_EVENT,
    EXIT_EXECUTOR_EVENT,
}


service Executor {
    i32 Helloworld(),
    //RS
    bool Exit(),

    // task
    bool StartTask(1: string task_ad),
    bool KillTask(1: i32 job_id, 2: i32 task_id),
    bool KillTaskForFT(1: i32 job_id, 2: i32 task_id),

    // vm heartbeat
    bool SendVMHeartbeat(1: string hb_vm_info_ad),

    //update image
    bool UpdateImage(1:string user, 2:string name, 3:i32 size),
}
