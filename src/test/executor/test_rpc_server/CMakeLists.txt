SET(THRIFT_SOURCE_PREFIX ${CMAKE_SOURCE_DIR}/src/proxy)

SET(TEST_THRIFT_SOURCE
    ${THRIFT_SOURCE_PREFIX}/test/gen-cpp/test_constants.h
    ${THRIFT_SOURCE_PREFIX}/test/gen-cpp/test_constants.cpp
    ${THRIFT_SOURCE_PREFIX}/test/gen-cpp/test_types.h
    ${THRIFT_SOURCE_PREFIX}/test/gen-cpp/test_types.cpp
    ${THRIFT_SOURCE_PREFIX}/test/gen-cpp/Test.h
    ${THRIFT_SOURCE_PREFIX}/test/gen-cpp/Test.cpp)
ADD_CUSTOM_COMMAND (
    OUTPUT ${TEST_THRIFT_SOURCE}
    COMMAND thrift --gen cpp -r -o ${CMAKE_SOURCE_DIR}/src/proxy/test
    ${CMAKE_SOURCE_DIR}/src/proxy/test/test.thrift
)

SET(SRC 
    ${TEST_THRIFT_SOURCE}
    service.cpp
    test.cpp)

ADD_EXECUTABLE(test_server ${SRC})

TARGET_LINK_LIBRARIES(test_server thrift)
TARGET_LINK_LIBRARIES(test_server pthread)
TARGET_LINK_LIBRARIES(test_server classad)
#TARGET_LINK_LIBRARIES(test gtest)
#TARGET_LINK_LIBRARIES(test boost_thread-mt)
#TARGET_LINK_LIBRARIES(test zookeeper_mt)

INSTALL(TARGETS test_server RUNTIME DESTINATION test_server)
