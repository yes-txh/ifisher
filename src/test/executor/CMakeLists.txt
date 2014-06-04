#ADD_SUBDIRECTORY(test_rpc_server)
SET(THRIFT_SOURCE_PREFIX ${CMAKE_SOURCE_DIR}/src/proxy)

SET(SCHEDULER_THRIFT_SOURCE
    ${THRIFT_SOURCE_PREFIX}/scheduler/gen-cpp/scheduler_constants.h 
    ${THRIFT_SOURCE_PREFIX}/scheduler/gen-cpp/scheduler_constants.cpp
    ${THRIFT_SOURCE_PREFIX}/scheduler/gen-cpp/scheduler_types.h
    ${THRIFT_SOURCE_PREFIX}/scheduler/gen-cpp/scheduler_types.cpp
    ${THRIFT_SOURCE_PREFIX}/scheduler/gen-cpp/Scheduler.h
    ${THRIFT_SOURCE_PREFIX}/scheduler/gen-cpp/Scheduler.cpp)
ADD_CUSTOM_COMMAND (
    OUTPUT ${SCHEDULER_THRIFT_SOURCE}
    COMMAND thrift --gen cpp -r -o ${CMAKE_SOURCE_DIR}/src/proxy/scheduler
    ${CMAKE_SOURCE_DIR}/src/proxy/scheduler/scheduler.thrift
)

SET(COLLECTOR_THRIFT_SOURCE
    ${THRIFT_SOURCE_PREFIX}/collector/gen-cpp/collector_constants.h
    ${THRIFT_SOURCE_PREFIX}/collector/gen-cpp/collector_constants.cpp
    ${THRIFT_SOURCE_PREFIX}/collector/gen-cpp/collector_types.h
    ${THRIFT_SOURCE_PREFIX}/collector/gen-cpp/collector_types.cpp
    ${THRIFT_SOURCE_PREFIX}/collector/gen-cpp/Collector.h
    ${THRIFT_SOURCE_PREFIX}/collector/gen-cpp/Collector.cpp)
ADD_CUSTOM_COMMAND (
    OUTPUT ${COLLECTOR_THRIFT_SOURCE}
    COMMAND thrift --gen cpp -r -o ${CMAKE_SOURCE_DIR}/src/proxy/collector
    ${CMAKE_SOURCE_DIR}/src/proxy/collector/collector.thrift
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



#SET (TEST_EXECUTOR
#     system_test)
     #container_test)


# test
#ADD_EXECUTABLE(system_test system_test.cpp ../../executor/system.cpp)
#ADD_EXECUTABLE(container_test container_test.cpp ../../slave/container.cpp
#                              ${COMMON_SOURCE} ../../slave/config.cpp
#                              ../../slave/system.cpp 
#                              ../../slave/message_manager.cpp)
#FOREACH(t ${TEST_EXECUTOR})
#    TARGET_LINK_LIBRARIES(${t} pthread)
#    TARGET_LINK_LIBRARIES(${t} log4cplus)
#    TARGET_LINK_LIBRARIES(${t} gtest)
#ENDFOREACH(t ${TEST_EXECUTOR})

#### start task ####
SET (START_TASK
     ${EXECUTOR_THRIFT_SOURCE}
     start_task.cpp)

ADD_EXECUTABLE(start-task ${START_TASK})
TARGET_LINK_LIBRARIES(start-task thrift)
TARGET_LINK_LIBRARIES(start-task classad)


#### kill task ####
SET (KILL_TASK
     ${EXECUTOR_THRIFT_SOURCE}
     kill_task.cpp)

ADD_EXECUTABLE(kill-task ${KILL_TASK})
TARGET_LINK_LIBRARIES(kill-task thrift)

#### stop task ####
#SET (STOP_TASK
#     ${EXECUTOR_THRIFT_SOURCE}
#     stop_task.cpp)

#ADD_EXECUTABLE(stop-task ${STOP_TASK})
#TARGET_LINK_LIBRARIES(stop-task thrift)


INSTALL(TARGETS ${TEST_EXECUTOR} RUNTIME DESTINATION test/executor)
INSTALL(TARGETS start-task RUNTIME DESTINATION bin)
INSTALL(TARGETS kill-task RUNTIME DESTINATION bin)

#INSTALL(TARGETS stop-task RUNTIME DESTINATION bin)
INSTALL(TARGETS ${TEST_EXECUTOR} RUNTIME DESTINATION test/executor)
INSTALL(TARGETS start-task RUNTIME DESTINATION bin)
INSTALL(TARGETS kill-task RUNTIME DESTINATION bin)
