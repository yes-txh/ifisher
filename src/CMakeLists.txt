SET(SRC ".")

INCLUDE_DIRECTORIES(
    ${THRIFT_INCLUDE_PATH}
    # ${JNI_INCLUDE_DIRS}  # for jni.h, jni.h use in /usr/local/include/hdfs/hdfs.h
    ${SRC}/)
# LINK_DIRECTORIES(${JVM_LIBRARAY_DIR})
# LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/third_party/hdfs/lib64)

ADD_SUBDIRECTORY(JobsManager)
ADD_SUBDIRECTORY(ResourceScheduler)
ADD_SUBDIRECTORY(executor)
#ADD_SUBDIRECTORY(executor_monitor)
#ADD_SUBDIRECTORY(submitter)
ADD_SUBDIRECTORY(test)
