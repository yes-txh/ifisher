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


# SET(COMMON_SOURCE
    # ../common/clynn/cond.cpp
    # ../common/clynn/thread.cpp)

## hdfs ##
# HOME
# SET(HADOOP_DIR /home/wm/work/hadoop/hadoop-1.0.4)
# SET(JAVA_HOME /usr/lib/java/jdk1.7.0_40)
# -I
# INCLUDE_DIRECTORIES($ENV{HADOOP_DIR}/src/c++/libhdfs)
# INCLUDE_DIRECTORIES($ENV{JAVA_HOME}/include)
# INCLUDE_DIRECTORIES($ENV{JAVA_HOME}/include/linux)
# -L
# LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/third_party/hdfs/lib64)
# LINK_DIRECTORIES(/etc/alternatives/jre/lib/amd64/server)

## hdfs new ##
# -I
INCLUDE_DIRECTORIES(${JNI_INCLUDE_DIRS})  # for jni.h, jni.h use in /usr/local/include/hdfs/hdfs.h
# -L
LINK_DIRECTORIES(${JVM_LIBRARAY_DIR})
LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/third_party/hdfs/lib64)



SET(SRC 
    ${RESOURCE_SCHEDULER_THRIFT_SOURCE}
    config.cpp
    restart_executor.cpp
    hdfs_wrapper.cpp
    work_thread.cpp
    checker.cpp
    main.cpp)

ADD_EXECUTABLE(executor_monitor ${SRC})

TARGET_LINK_LIBRARIES(executor_monitor pthread)
TARGET_LINK_LIBRARIES(executor_monitor thrift)
TARGET_LINK_LIBRARIES(executor_monitor log4cplus)
TARGET_LINK_LIBRARIES(executor_monitor gflags)
TARGET_LINK_LIBRARIES(executor_monitor classad)
# hdfs
TARGET_LINK_LIBRARIES(executor_monitor hdfs)
TARGET_LINK_LIBRARIES(executor_monitor jvm)


INSTALL(TARGETS executor_monitor RUNTIME DESTINATION bin)
