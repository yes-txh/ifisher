SET(THRIFT_SOURCE_PREFIX ${CMAKE_SOURCE_DIR}/src/proxy)

SET(JOBS_MANAGER_THRIFT_SOURCE
    ${THRIFT_SOURCE_PREFIX}/JobsManager/gen-cpp/jobs_manager_constants.h 
    ${THRIFT_SOURCE_PREFIX}/JobsManager/gen-cpp/jobs_manager_constants.cpp
    ${THRIFT_SOURCE_PREFIX}/JobsManager/gen-cpp/jobs_manager_types.h
    ${THRIFT_SOURCE_PREFIX}/JobsManager/gen-cpp/jobs_manager_types.cpp
    ${THRIFT_SOURCE_PREFIX}/JobsManager/gen-cpp/JobsManager.h
    ${THRIFT_SOURCE_PREFIX}/JobsManager/gen-cpp/JobsManager.cpp)

ADD_CUSTOM_COMMAND (
    OUTPUT ${JOBS_MANAGER_THRIFT_SOURCE}
    COMMAND thrift --gen cpp -r -o ${CMAKE_SOURCE_DIR}/src/proxy/JobsManager
    ${CMAKE_SOURCE_DIR}/src/proxy/JobsManager/jobs_manager.thrift
)

SET(RESOURCE_JOBS_MANAGER_THRIFT_SOURCE
    ${THRIFT_SOURCE_PREFIX}/ResourceScheduler/gen-cpp/resource_scheduler_constants.h
    ${THRIFT_SOURCE_PREFIX}/ResourceScheduler/gen-cpp/resource_scheduler_constants.cpp
    ${THRIFT_SOURCE_PREFIX}/ResourceScheduler/gen-cpp/resource_scheduler_types.h
    ${THRIFT_SOURCE_PREFIX}/ResourceScheduler/gen-cpp/resource_scheduler_types.cpp
    ${THRIFT_SOURCE_PREFIX}/ResourceScheduler/gen-cpp/ResourceScheduler.h
    ${THRIFT_SOURCE_PREFIX}/ResourceScheduler/gen-cpp/ResourceScheduler.cpp)
ADD_CUSTOM_COMMAND (
    OUTPUT ${RESOURCE_JOBS_MANAGER_THRIFT_SOURCE}
    COMMAND thrift --gen cpp -r -o ${CMAKE_SOURCE_DIR}/src/proxy/ResourceScheduler
    ${CMAKE_SOURCE_DIR}/src/proxy/ResourceScheduler/resource_scheduler.thrift
)

SET(EXECUTOR_THRIFT_SOURCE
    ${THRIFT_SOURCE_PREFIX}/executor/gen-cpp/executor_constants.h
    ${THRIFT_SOURCE_PREFIX}/executor/gen-cpp/executor_constants.cpp
    ${THRIFT_SOURCE_PREFIX}/executor/gen-cpp/executor_types.h
    ${THRIFT_SOURCE_PREFIX}/executor/gen-cpp/executor_types.cpp
    ${THRIFT_SOURCE_PREFIX}/executor/gen-cpp/Executor.h
    ${THRIFT_SOURCE_PREFIX}/executor/gen-cpp/Executor.cpp)
ADD_CUSTOM_COMMAND (
    OUTPUT ${EXECUTOR_THRIFT_SOURCE}
    COMMAND thrift --gen cpp -r -o ${CMAKE_SOURCE_DIR}/src/proxy/executor
    ${CMAKE_SOURCE_DIR}/src/proxy/executor/executor.thrift
    COMMAND thrift --gen py -r -o ${CMAKE_SOURCE_DIR}/src/proxy/executor
    ${CMAKE_SOURCE_DIR}/src/proxy/executor/executor.thrift
)


#SET(COMMON_SOURCE
#    ../proxy/resource_scheduler_wrapper.cpp)

SET(SRC 
    # ${COMMON_SOURCE}
    ${JOBS_MANAGER_THRIFT_SOURCE}
    ${RESOURCE_JOBS_MANAGER_THRIFT_SOURCE}
    ${EXECUTOR_THRIFT_SOURCE}
    config.cpp
    task.cpp
    batch_task.cpp
    job.cpp
    batch_job.cpp
    job_pool.cpp
    group.cpp
    group_pool.cpp
    job_parser.cpp
    job_scheduler.cpp
    default_scheduler.cpp
    wd_scheduler.cpp
    job_master.cpp
    image_manager.cpp
    app_manager.cpp
    handler.cpp
    event.cpp
    dispatcher.cpp
    work_thread.cpp
    jobs_manager_service.cpp
    jobs_manager_main.cpp)

ADD_EXECUTABLE(JobsManager ${SRC})

TARGET_LINK_LIBRARIES(JobsManager thrift)
TARGET_LINK_LIBRARIES(JobsManager pthread)
TARGET_LINK_LIBRARIES(JobsManager log4cplus)
TARGET_LINK_LIBRARIES(JobsManager classad)
TARGET_LINK_LIBRARIES(JobsManager gflags)
#TARGET_LINK_LIBRARIES(JobsManager zookeeper_mt)


INSTALL(TARGETS JobsManager RUNTIME DESTINATION bin)
