// =============================================================================
// Copyright (c) 2010, Tencent, China
// Version: 0.1

// $Author: mavisluo $
// $Revision: 43791 $
// $Description:test for zookeeper$
// $Id: zk_common_test.cpp 43791 2011-06-02 08:11:46Z mavisluo $
// =============================================================================

#include <gtest/gtest.h>
#include <gflags/gflags.h>
#include <map>
#include "tborg/conf_manager/zk_common.h"
#include "tborg/conf_manager/watcher.h"

void PrintMap(std::map<std::string, std::string> one_map) {
    for (std::map<std::string, std::string>::iterator it
         = one_map.begin(); it != one_map.end(); ++it) {
        std::cout << it->first << "-----" << it->second << std::endl;
    }
}


TEST(TestZookeeperCommon, TestCreateNode) {
    ZookeeperCommon my_zk;
    int rt = my_zk.Init("xaec-test_for_zk_common");
    if (rt < 0) {
        LOG(ERROR) << "connect zk failed.";
    } else {
        EXPECT_EQ(rt, 0);
        rt = my_zk.CreateNodeForce("/zk/xaec/tborg/xaec-test_for_zk_common/tns/mavisluo");
        std::cout << "=============================" << my_zk.GetZKPrefix() << std::endl;
        // sleep(20);
        EXPECT_EQ(rt, 0);

        rt = my_zk.CreateNode("/zk/xaec/tborg/xaec-test_for_zk_common"
                              "/tns/mavisluo/no_node/no_node");
        EXPECT_EQ(rt, -1);
        /// delete the node that created just now
        rt = my_zk.DeleteNode("/zk/xaec/tborg/xaec-test_for_zk_common/tns/mavisluo");
        EXPECT_EQ(rt, 0);
    }
}

TEST(TestZookeeperCommon, TestCreateNodeWithValue) {
    ZookeeperCommon my_zk;
    std::string value;
    int rt = my_zk.Init("xaec-test_for_zk_common");
    if (rt < 0) {
        LOG(ERROR) << "connect zk failed.";
    } else {
        EXPECT_EQ(rt, 0);
        my_zk.DeleteNode("/zk/xaec/tborg/xaec-test_for_zk_common/tns/mavisluo");

        rt = my_zk.CreateNodeWithValue("/zk/xaec/tborg/xaec-test_for_zk_common"
                                       "/tns/mavisluotest", "test_value");
        EXPECT_EQ(rt, 0);
        rt = my_zk.CreateNodeWithValue("/zk/xaec/tborg/xaec-test_for_zk_common"
                                       "/tns/mavisluo", "test_value_2");
        EXPECT_EQ(rt, 0);
        rt = my_zk.GetValueOfNode("/zk/xaec/tborg/xaec-test_for_zk_common/tns/mavisluo", &value);
        std::cout << value << std::endl;
        EXPECT_EQ(value, "test_value_2");

        rt = my_zk.CreateNodeWithValue("/zk/xaec/tborg/xaec-test_for_zk_common/"
                                       "tns/mavisluo/no_node/no_node/no_node/"
                                       "no_node/no_node", "test_value");
        std::cout << value << std::endl;
        EXPECT_EQ(rt, -1);
        std::string test_str= "test str";
        my_zk.SetValueOfNode("/zk/xaec/tborg/xaec-test_for_zk_common/tns/mavisluo", test_str);
        std::string result;
        my_zk.GetValueOfNode("/zk/xaec/tborg/xaec-test_for_zk_common/tns/mavisluo", &result);
        EXPECT_EQ(test_str, result);

        my_zk.DeleteNode("/zk/xaec/tborg/xaec-test_for_zk_common/tns/mavisluo");
    }
}
TEST(TestZookeeperCommon, TestDeleteNode) {
    ZookeeperCommon my_zk;
    std::string value;
    int rt = my_zk.Init("xaec-test_for_zk_common");
    if (rt < 0) {
        LOG(ERROR) << "connect zk failed.";
    } else {
        EXPECT_EQ(rt, 0);
        rt = my_zk.CreateNodeWithValue("/zk/xaec/tborg/xaec-test_for_zk_common"
                                       "/tns/mavisluo", "test_value");
        EXPECT_EQ(rt, 0);
        rt = my_zk.CheckPathExist("/zk/xaec/tborg/xaec-test_for_zk_common/tns/mavisluo");
        EXPECT_EQ(rt, 0);

        rt = my_zk.DeleteNode("/zk/xaec/tborg/xaec-test_for_zk_common/tns/mavisluo");
        EXPECT_EQ(rt, 0);

        rt = my_zk.CheckPathExist("/zk/xaec/tborg/xaec-test_for_zk_common/tns/mavisluo");
        EXPECT_EQ(rt, -1);
    }
}

TEST(TestZookeeperCommon, TestDeleteNodeForce) {
    ZookeeperCommon my_zk;
    std::string value;
    int rt = my_zk.Init("xaec-test_for_zk_common");
    if (rt < 0) {
        LOG(ERROR) << "connect zk failed.";
    } else {
        EXPECT_EQ(rt, 0);
        rt = my_zk.CreateNodeForce("/zk/xaec/tborg/xaec-test_for_zk_common/tns/test_del/job_0");
        EXPECT_EQ(rt, 0);
        rt = my_zk.CreateNodeForce("/zk/xaec/tborg/xaec-test_for_zk_common/tns/test_del/job_1");
        EXPECT_EQ(rt, 0);
        rt = my_zk.CreateNodeForce("/zk/xaec/tborg/xaec-test_for_zk_common/tns/test_del/job_1"
                                   "/test");
        EXPECT_EQ(rt, 0);
        rt = my_zk.DeleteNodeForce("/zk/xaec/tborg/xaec-test_for_zk_common/tns/test_del");
        EXPECT_EQ(rt, -1);
        rt = my_zk.DeleteNode("/zk/xaec/tborg/xaec-test_for_zk_common/tns/test_del/job_1/test");
        EXPECT_EQ(rt, 0);

        rt = my_zk.DeleteNodeForce("/zk/xaec/tborg/xaec-test_for_zk_common/tns/test_del");
        EXPECT_EQ(rt, 0);

        rt = my_zk.CheckPathExist("/zk/xaec/tborg/xaec-test_for_zk_common/tns/test_del");
        EXPECT_EQ(rt, -1);
    }
}

TEST(TestZookeeperCommon, TestDeleteNodeForceRecursively) {
    ZookeeperCommon my_zk;
    std::string value;
    int rt = my_zk.Init("xaec-test_for_zk_common");
    if (rt < 0) {
        LOG(ERROR) << "connect zk failed.";
    } else {
        EXPECT_EQ(rt, 0);
        rt = my_zk.CreateNodeForce("/zk/xaec/tborg/xaec-test_for_zk_common/tns/node/child");
        EXPECT_EQ(rt, 0);
        rt = my_zk.CreateNodeForce("/zk/xaec/tborg/xaec-test_for_zk_common/tns/node/child/grand");
        EXPECT_EQ(rt, 0);
        rt = my_zk.CreateNodeForce("/zk/xaec/tborg/xaec-test_for_zk_common/tns/nodei"
                                   "/child/grand/anther");
        EXPECT_EQ(rt, 0);

        rt = my_zk.DeleteNodeForceRecursively("/zk/xaec/tborg/xaec-test_for_zk_common/tns/node");
        EXPECT_EQ(rt, 0);

        rt = my_zk.CheckPathExist("/zk/xaec/tborg/xaec-test_for_zk_common/tns/node");
        EXPECT_EQ(rt, -1);
    }
}

TEST(TestZookeeperCommon, TestCreateNodeForce) {
    ZookeeperCommon my_zk;
    std::string value;
    int rt = my_zk.Init("xaec-test_for_zk_common");
    if (rt < 0) {
        LOG(ERROR) << "connect zk failed.";
    } else {
        EXPECT_EQ(rt, 0);
        std::map<std::string, std::string> children;
        rt = my_zk.CreateNodeForce("/zk/xaec/tborg/xaec-test_for_zk_common/tns/mavisluo"
                                   "/second_node");
        EXPECT_EQ(rt, 0);
        rt = my_zk.CheckPathExist("/zk/xaec/tborg/xaec-test_for_zk_common/tns/mavisluo"
                                  "/second_node");
        EXPECT_EQ(rt, 0);
        my_zk.DeleteNode("/zk/xaec/tborg/xaec-test_for_zk_common/tns/mavisluo/second_node");
        my_zk.DeleteNode("/zk/xaec/tborg/xaec-test_for_zk_common/tns/mavisluo");
    }
}

TEST(TestZookeeperCommon, TestGetChildren) {
    ZookeeperCommon my_zk;
    std::string value;
    int rt = my_zk.Init("xaec-test_for_zk_common");
    if (rt < 0) {
        LOG(ERROR) << "connect zk failed.";
    } else {
        EXPECT_EQ(rt, 0);
        std::map<std::string, std::string> children;
        my_zk.CreateNodeForce("/zk/xaec/tborg/xaec-test_for_zk_common/tns/mavisluo/job_0");
        my_zk.CreateNodeForce("/zk/xaec/tborg/xaec-test_for_zk_common/tns/mavisluo/job_1");
        rt = my_zk.GetChildren("/zk/xaec/tborg/xaec-test_for_zk_common/tns/mavisluo", &children);
        PrintMap(children);
        EXPECT_EQ(rt, 2);
        EXPECT_EQ(static_cast<int>(children.size()), 2);
        rt = my_zk.GetChildren("/zk/xaec/tborg/xaec-test_for_zk_common/tns/unknown_mavisluo",
                               &children);
        EXPECT_EQ(rt, -1);
        /// delete the test nodes
        my_zk.DeleteNode("/zk/xaec/tborg/xaec-test_for_zk_common/tns/mavisluo/job_0");
        my_zk.DeleteNode("/zk/xaec/tborg/xaec-test_for_zk_common/tns/mavisluo/job_1");
        my_zk.DeleteNode("/zk/xaec/tborg/xaec-test_for_zk_common/tns/mavisluo");
    }
}

TEST(TestZookeeperCommon, TestWCheckPathExist) {
    ZookeeperCommon my_zk;
    std::string value;
    int rt = my_zk.Init("xaec-test_for_zk_common");
    if (rt < 0) {
        LOG(ERROR) << "connect zk failed.";
    } else {
        EXPECT_EQ(rt, 0);
        std::map<std::string, std::string> children;
        rt = my_zk.CreateNodeForce("/zk/xaec/tborg/xaec-test_for_zk_common/tns/mavisluo"
                                   "/second_node");
        EXPECT_EQ(rt, 0);
        rt = my_zk.WCheckPathExist("/zk/xaec/tborg/xaec-test_for_zk_common/tns/mavisluo"
                                   "/second_node", &Watcher);
        EXPECT_EQ(rt, 0);

        rt = my_zk.SetValueOfNode("/zk/xaec/tborg/xaec-test_for_zk_common/tns/mavisluo"
                                  "/second_node", "new_value");
        rt = my_zk.SetValueOfNode("/zk/xaec/tborg/xaec-test_for_zk_common/tns/mavisluo"
                                  "/second_node", "new_value_2");
        my_zk.DeleteNode("/zk/xaec/tborg/xaec-test_for_zk_common/tns/mavisluo/second_node");
        my_zk.DeleteNode("/zk/xaec/tborg/xaec-test_for_zk_common/tns/mavisluo");
    }
}

TEST(TestZookeeperCommon, TestWGetValueOfNode) {
    ZookeeperCommon my_zk;
    std::string value;
    int rt = my_zk.Init("xaec-test_for_zk_common");
    if (rt < 0) {
        LOG(ERROR) << "connect zk failed.";
    } else {
        EXPECT_EQ(rt, 0);
        std::map<std::string, std::string> children;
        rt = my_zk.CreateNodeForce("/zk/xaec/tborg/xaec-test_for_zk_common/tns/"
                                   "mavisluo/second_node");
        EXPECT_EQ(rt, 0);
        rt = my_zk.SetValueOfNode("/zk/xaec/tborg/xaec-test_for_zk_common/tns/"
                                  "mavisluo/second_node", "frst_value");

        rt = my_zk.WGetValueOfNode("/zk/xaec/tborg/xaec-test_for_zk_common/tns/"
                                   "mavisluo/second_node", &Watcher, &value);
        LOG(INFO) << "===========" << value;
        EXPECT_EQ(rt, 0);

        rt = my_zk.CheckPathExist("/zk/xaec/tborg/xaec-test_for_zk_common/tns/"
                                   "mavisluo/second_node");
        EXPECT_EQ(rt, 0);
        rt = my_zk.SetValueOfNode("/zk/xaec/tborg/xaec-test_for_zk_common/tns/"
                                  "mavisluo/second_node", "new_value");
        rt = my_zk.SetValueOfNode("/zk/xaec/tborg/xaec-test_for_zk_common/tns/"
                                "mavisluo/second_node", "new_value_2");
        my_zk.DeleteNode("/zk/xaec/tborg/xaec-test_for_zk_common/tns/mavisluo/second_node");
        my_zk.DeleteNode("/zk/xaec/tborg/xaec-test_for_zk_common/tns/mavisluo");
    }
}

int main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    if (google::ParseCommandLineFlags(&argc, &argv, true)) {
        for (int i = 0; i < argc; ++i) {
            printf("arg[%d]=%s\n", i, argv[i]);
        }
    } else {
        return  EXIT_FAILURE;
    }

    /// first delete all node to use in our tests
    ZookeeperCommon zk;
    zk.Init("xaec-test_for_zk_common");
    zk.DeleteNode("/zk/xaec/tborg/xaec-test_for_zk_common/tns/mavisluo/my_test_job/0");
    zk.DeleteNode("/zk/xaec/tborg/xaec-test_for_zk_common/tns/mavisluo/my_test_job/1");
    zk.DeleteNode("/zk/xaec/tborg/xaec-test_for_zk_common/tns/mavisluo/my_test_job");
    zk.DeleteNode("/zk/xaec/tborg/xaec-test_for_zk_common/tns/mavisluo");
    return RUN_ALL_TESTS();
}
