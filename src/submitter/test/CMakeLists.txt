SET(THRIFT_SOURCE_PREFIX ${CMAKE_SOURCE_DIR}/src/proxy)
  
SET(MASTER_THRIFT_SOURCE
    ${THRIFT_SOURCE_PREFIX}/master/gen-cpp/master_constants.h
    ${THRIFT_SOURCE_PREFIX}/master/gen-cpp/master_constants.cpp
    ${THRIFT_SOURCE_PREFIX}/master/gen-cpp/master_types.h
    ${THRIFT_SOURCE_PREFIX}/master/gen-cpp/master_types.cpp
    ${THRIFT_SOURCE_PREFIX}/master/gen-cpp/Master.h
    ${THRIFT_SOURCE_PREFIX}/master/gen-cpp/Master.cpp)
ADD_CUSTOM_COMMAND (
    OUTPUT ${MASTER_THRIFT_SOURCE}
    COMMAND thrift --gen cpp -r -o ${CMAKE_SOURCE_DIR}/src/proxy/master
    ${CMAKE_SOURCE_DIR}/src/proxy/master/master.thrift
)

INCLUDE_DIRECTORIES(
$ENV{HADOOP_HOME}/src/c++/libhdfs
)
INCLUDE_DIRECTORIES(
$ENV{JAVA_HOME}/include
)
INCLUDE_DIRECTORIES(
$ENV{JAVA_HOME}/include/linux
)

INCLUDE_DIRECTORIES(
$ENV{JAVA_HOME}/jre/lib/amd64/server
)
INCLUDE_DIRECTORIES(
$ENV{HADOOP_HOME}/build/c++/Linux-amd64-64/lib
)

SET (SUBMITTER
   ${MASTER_THRIFT_SOURCE}  
   submitter.cpp
   cmd_parse.cpp
   app.cpp
   app_file.cpp
   app_config.cpp
   user.cpp
   config.cpp)
 
ADD_EXECUTABLE(submitter ${SUBMITTER})

TARGET_LINK_LIBRARIES(submitter thrift)
TARGET_LINK_LIBRARIES(submitter classad)
TARGET_LINK_LIBRARIES(submitter log4cplus)
TARGET_LINK_LIBRARIES(submitter hdfs)
TARGET_LINK_LIBRARIES(submitter jvm)
TARGET_LINK_LIBRARIES(submitter gflags)

SET (APPFILETEST
   app_file_test.cpp
   app_file.cpp
   )

ADD_EXECUTABLE(app_file_test ${APPFILETEST})

#TARGET_LINK_LIBRARIES(submitter thrift)
#TARGET_LINK_LIBRARIES(submitter classad)
TARGET_LINK_LIBRARIES(app_file_test log4cplus)
TARGET_LINK_LIBRARIES(app_file_test hdfs)
TARGET_LINK_LIBRARIES(app_file_test jvm)

