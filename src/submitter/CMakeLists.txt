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

#INCLUDE_DIRECTORIES(
#$ENV{HADOOP_HOME}/src/c++/libhdfs
#)
#INCLUDE_DIRECTORIES(
#$ENV{JAVA_HOME}/include
#)
#INCLUDE_DIRECTORIES(
#$ENV{JAVA_HOME}/include/linux
#)

#INCLUDE_DIRECTORIES(
#$ENV{JAVA_HOME}/jre/lib/amd64/server
#)
#INCLUDE_DIRECTORIES(
#$ENV{HADOOP_HOME}/build/c++/Linux-amd64-64/lib
#)

## hdfs new ##
# -I
INCLUDE_DIRECTORIES(${JNI_INCLUDE_DIRS})  # for jni.h, jni.h use in /usr/local/include/hdfs/hdfs.h
# -L
LINK_DIRECTORIES(${JVM_LIBRARAY_DIR})
LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/third_party/hdfs/lib64)


SET (SUBMITTER
   ${JOBS_MANAGER_THRIFT_SOURCE}  
   submitter.cpp
   cmd_parse.cpp
   image.cpp
   app.cpp
   job.cpp
   config.cpp
   hdfs_wrapper.cpp)
 
ADD_EXECUTABLE(submitter ${SUBMITTER})

TARGET_LINK_LIBRARIES(submitter thrift)
TARGET_LINK_LIBRARIES(submitter classad)
TARGET_LINK_LIBRARIES(submitter log4cplus)
TARGET_LINK_LIBRARIES(submitter hdfs)
TARGET_LINK_LIBRARIES(submitter jvm)
TARGET_LINK_LIBRARIES(submitter gflags)

INSTALL(TARGETS submitter RUNTIME DESTINATION bin)
                                                             

#for test
#SET (APPFILETEST
#   app_file_test.cpp
#   app_file.cpp
#   )

#ADD_EXECUTABLE(app_file_test ${APPFILETEST})

#TARGET_LINK_LIBRARIES(submitter thrift)
#TARGET_LINK_LIBRARIES(submitter classad)
#TARGET_LINK_LIBRARIES(app_file_test log4cplus)
#TARGET_LINK_LIBRARIES(app_file_test hdfs)
#TARGET_LINK_LIBRARIES(app_file_test jvm)

