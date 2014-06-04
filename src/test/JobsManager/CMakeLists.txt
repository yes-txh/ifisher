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

SET(RESOURCE_SCHEDULER_THRIFT_SOURCE
    ${THRIFT_SOURCE_PREFIX}/ResourceScheduler/gen-cpp/resource_scheduler_constants.h
    ${THRIFT_SOURCE_PREFIX}/ResourceScheduler/gen-cpp/resource_scheduler_constants.cpp
    ${THRIFT_SOURCE_PREFIX}/ResourceScheduler/gen-cpp/resource_scheduler_types.h
    ${THRIFT_SOURCE_PREFIX}/ResourceScheduler/gen-cpp/resource_scheduler_types.cpp
    ${THRIFT_SOURCE_PREFIX}/ResourceScheduler/gen-cpp/ResourceScheduler.h
    ${THRIFT_SOURCE_PREFIX}/ResourceScheduler/gen-cpp/ResourceScheduler.cpp)
ADD_CUSTOM_COMMAND (
    OUTPUT ${RESOURCE_SCHEDULER_THRIFT_SOURCE}
    COMMAND thrift --gen cpp -r -o
${CMAKE_SOURCE_DIR}/src/proxy/ResourceScheduler
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

#### submit job ####
SET (SUBMIT_JOB
     ${JOBS_MANAGER_THRIFT_SOURCE}
     submit_job_xml.cpp)

ADD_EXECUTABLE(submit-job ${SUBMIT_JOB})
TARGET_LINK_LIBRARIES(submit-job thrift)
TARGET_LINK_LIBRARIES(submit-job classad)

#### submit job ####
SET (SUBMIT_JOB_DAEMON
     ${JOBS_MANAGER_THRIFT_SOURCE}
     submit_job_daemon.cpp)

ADD_EXECUTABLE(submit-job-daemon ${SUBMIT_JOB_DAEMON})
TARGET_LINK_LIBRARIES(submit-job-daemon thrift)
TARGET_LINK_LIBRARIES(submit-job-daemon classad)

# INSTALL(TARGETS ${TEST_EXECUTOR} RUNTIME DESTINATION test/executor)
INSTALL(TARGETS submit-job RUNTIME DESTINATION test/JobsManager)
INSTALL(TARGETS submit-job-daemon RUNTIME DESTINATION test/JobsManager)

