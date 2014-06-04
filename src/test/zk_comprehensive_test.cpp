// =============================================================================
// Copyright (c) 2010, Tencent, China
// Version: 0.1

// $Author: mavisluo $
// $Revision: 17158 $
// $Description:test for zookeeper$
// $Id: zk_common_test.cpp 17158 2010-12-28 11:49:12Z mavisluo $
// =============================================================================

#include <gtest/gtest.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <gflags/gflags.h>
#include <map>
#include <list>
#include <string>
#include <vector>
#include "tborg/conf_manager/zk_common.h"
#include "tborg/include/get_hostip.h"
#include "common/config/cflags.hpp"
#include "tborg/collector/collector_conf.h"
#include "tborg/conf_manager/dynamic_configuration.h"
#include "tborg/conf_manager/conf_manager.h"
#include "common/system/concurrency/atomic/atomic.hpp"

/// ip:port of zk servers
DEFINE_string(zk_servers, "", "ip:port of zk servers");

std::string test_node = "/zk/xaec/tborg/" + FLAGS_cluster_name +
                        "/tns/mavisluo";

class MyEnvironment : public testing::Environment
{
    public:
        virtual void SetUp() {
            std::cout << "MyEnvironment SetUP" << std::endl;
            FLAGS_cluster_name = "xaec-comprehensive_test_zk_" + get_hostip();
            /// create test nodes
            ConfManager my_zk;

            int rt = my_zk.Init(FLAGS_cluster_name, FLAGS_zk_servers);
            if (rt < 0) {
                std::cout << "tns_test init failed." << std::endl;
                return;
            } else {
                my_zk.CreateDefaultCluster();
                /// set some config items
                my_zk.SetOneConf("global", "collector_endpoint", "10.6.207.227:9618");
                my_zk.SetOneConf("global", "scheduler_endpoint", "10.6.207.227:9618");
                my_zk.SetOneConf("global", "scheduler_http_endpoint", "10.6.207.227:9618");
                my_zk.SetOneConf("global", "scheduler_endpoint_for_submitter", "10.6.207.227:9618");
                my_zk.SetOneConf("collector", "borg_dbhost", "10.6.207.228");
            }
        }

        virtual void TearDown() {
            std::cout << "MyEnvironment TearDown" << std::endl;
            ZookeeperCommon my_zk;
            int rt = my_zk.Init(FLAGS_cluster_name, FLAGS_zk_servers);
            if (rt < 0) {
                std::cout << "zk server init failed." << std::endl;
            } else {
                /// delete all test ndoes
                my_zk.DeleteNodeForceRecursively("/zk/xaec/tborg/" + FLAGS_cluster_name);
            }
        }
};

void PrintMap(std::map<std::string, std::string> one_map) {
    for (std::map<std::string, std::string>::iterator it
         = one_map.begin(); it != one_map.end(); ++it) {
        std::cout << it->first << "-----" << it->second << std::endl;
    }
}

/// test: create a zk node
TEST(TestZookeeperCommon, TestCreateNode) {
    ZookeeperCommon my_zk;
    int rt = my_zk.Init(FLAGS_cluster_name, FLAGS_zk_servers);
    if (rt < 0) {
        LOG(ERROR) << "connect zk failed.";
    } else {
        EXPECT_EQ(rt, 0);
        /// create a normal node, the result will be 0
        rt = my_zk.CreateNode(test_node);
        EXPECT_EQ(rt, 0);
        rt = my_zk.CheckPathExist(test_node);
        EXPECT_EQ(rt, 0);
        /// if you create a node already exists, the result is also 0
        rt = my_zk.CreateNode(test_node);
        EXPECT_EQ(rt, 0);
        rt = my_zk.CheckPathExist(test_node);
        EXPECT_EQ(rt, 0);
        /// if you create a node which has no parent, the result is -1
        rt = my_zk.CreateNode(test_node + "/no_parent/node");
        EXPECT_EQ(rt, -1);
        rt = my_zk.CheckPathExist(test_node + "/no_parent/node");
        EXPECT_EQ(rt, -1);
        /// delete the node that created just now
        rt = my_zk.DeleteNodeForceRecursively(test_node);
        EXPECT_EQ(rt, 0);
        rt = my_zk.CheckPathExist(test_node);
        EXPECT_EQ(rt, -1);
    }
}

/// test: check if a node(path) exists
TEST(TestZookeeperCommon, CheckPathExist) {
    ZookeeperCommon my_zk;
    int rt = my_zk.Init(FLAGS_cluster_name, FLAGS_zk_servers);
    if (rt < 0) {
        LOG(ERROR) << "connect zk failed.";
    } else {
        EXPECT_EQ(rt, 0);
        /// create a node
        rt = my_zk.CreateNode(test_node);
        EXPECT_EQ(rt, 0);
        rt = my_zk.CheckPathExist(test_node);
        EXPECT_EQ(rt, 0);
        /// delete the node
        rt = my_zk.DeleteNode(test_node);
        EXPECT_EQ(rt, 0);
        rt = my_zk.CheckPathExist(test_node);
        EXPECT_EQ(rt, -1);
    }
}

/// test: create a node with its value
TEST(TestZookeeperCommon, TestCreateNodeWithValue) {
    ZookeeperCommon my_zk;
    std::string value;
    int rt = my_zk.Init(FLAGS_cluster_name, FLAGS_zk_servers);
    if (rt < 0) {
        LOG(ERROR) << "connect zk failed.";
    } else {
        EXPECT_EQ(rt, 0);
        // in order to ensure the test node exists, delete it first
        my_zk.DeleteNode(test_node);

        rt = my_zk.CreateNodeWithValue(test_node, "test_value");
        EXPECT_EQ(rt, 0);
        rt = my_zk.GetValueOfNode(test_node, &value);
        EXPECT_EQ(value, "test_value");
        rt = my_zk.CreateNodeWithValue(test_node, "test_value_2");
        EXPECT_EQ(rt, 0);
        rt = my_zk.GetValueOfNode(test_node, &value);
        // std::cout << value << std::endl;
        EXPECT_EQ(value, "test_value_2");
        // create with value will fail if you create a node has no parent
        rt = my_zk.CreateNodeWithValue(test_node +
                                      "/no_node/no_node/no_node/no_node/no_node",
                                      "test_value");
        // std::cout << value << std::endl;
        EXPECT_EQ(rt, -1);
        my_zk.DeleteNodeForceRecursively(test_node);
    }
}

/// test: set a node with its value
TEST(TestZookeeperCommon, TestSetValueOfNode) {
    ZookeeperCommon my_zk;
    std::string value;
    int rt = my_zk.Init(FLAGS_cluster_name, FLAGS_zk_servers);
    if (rt < 0) {
        LOG(ERROR) << "connect zk failed.";
    } else {
        EXPECT_EQ(rt, 0);

        rt = my_zk.CreateNode(test_node);
        EXPECT_EQ(rt, 0);
        rt = my_zk.SetValueOfNode(test_node, "test_value");
        EXPECT_EQ(rt, 0);
        rt = my_zk.GetValueOfNode(test_node, &value);
        EXPECT_EQ(value, "test_value");
        rt = my_zk.SetValueOfNode(test_node, "test_value_2");
        EXPECT_EQ(rt, 0);
        rt = my_zk.GetValueOfNode(test_node, &value);
        // std::cout << value << std::endl;
        EXPECT_EQ(value, "test_value_2");
        // create with value will fail if you create a node has no parent
        rt = my_zk.CreateNodeWithValue(test_node +
                                       "/no_node/no_node/no_node/no_node/no_node",
                                       "test_value");
        // std::cout << value << std::endl;
        EXPECT_EQ(rt, -1);
        my_zk.DeleteNodeForceRecursively(test_node);
    }
}

/// test: delete a node
TEST(TestZookeeperCommon, TestDeleteNode) {
    ZookeeperCommon my_zk;
    std::string value;
    int rt = my_zk.Init(FLAGS_cluster_name, FLAGS_zk_servers);
    if (rt < 0) {
        LOG(ERROR) << "connect zk failed.";
    } else {
        EXPECT_EQ(rt, 0);
        // in order to ensure the test node exists, delete it first
        my_zk.DeleteNode(test_node);
        rt = my_zk.CreateNodeWithValue(test_node, "test_value");
        EXPECT_EQ(rt, 0);
        rt = my_zk.CheckPathExist(test_node);
        EXPECT_EQ(rt, 0);

        rt = my_zk.DeleteNode(test_node);
        EXPECT_EQ(rt, 0);
        // delete a node which not exist
        rt = my_zk.DeleteNode(test_node);
        EXPECT_EQ(rt, 0);

        rt = my_zk.CheckPathExist(test_node);
        EXPECT_EQ(rt, -1);
        my_zk.DeleteNodeForceRecursively(test_node);
    }
}

/// test: delete a node forcelly
TEST(TestZookeeperCommon, TestDeleteNodeForce) {
    ZookeeperCommon my_zk;
    std::string value;
    int rt = my_zk.Init(FLAGS_cluster_name, FLAGS_zk_servers);
    if (rt < 0) {
        LOG(ERROR) << "connect zk failed.";
    } else {
        EXPECT_EQ(rt, 0);
        rt = my_zk.CreateNodeForce(test_node + "/job_0");
        EXPECT_EQ(rt, 0);
        rt = my_zk.CreateNodeForce(test_node + "/job_1");
        EXPECT_EQ(rt, 0);
        rt = my_zk.CreateNodeForce(test_node + "/job_1/test");
        EXPECT_EQ(rt, 0);
        rt = my_zk.DeleteNodeForce(test_node);
        EXPECT_EQ(rt, -1);
        rt = my_zk.DeleteNode(test_node + "/job_1/test");
        EXPECT_EQ(rt, 0);

        rt = my_zk.DeleteNodeForce(test_node);
        EXPECT_EQ(rt, 0);
        rt = my_zk.DeleteNodeForce(test_node);
        EXPECT_EQ(rt, 0);
        rt = my_zk.CheckPathExist(test_node);
        EXPECT_EQ(rt, -1);
        my_zk.DeleteNodeForceRecursively(test_node);
    }
}

/// test: delete all node force recursively according to a path
TEST(TestZookeeperCommon, TestDeleteNodeForceRecursively) {
    ZookeeperCommon my_zk;
    std::string value;
    int rt = my_zk.Init(FLAGS_cluster_name, FLAGS_zk_servers);
    if (rt < 0) {
        LOG(ERROR) << "connect zk failed.";
    } else {
        EXPECT_EQ(rt, 0);
        rt = my_zk.CreateNodeForce(test_node + "/child");
        EXPECT_EQ(rt, 0);
        rt = my_zk.CreateNodeForce(test_node + "/child/grand");
        EXPECT_EQ(rt, 0);
        rt = my_zk.CreateNodeForce(test_node + "/child/grand/anther");
        EXPECT_EQ(rt, 0);

        rt = my_zk.DeleteNodeForceRecursively(test_node);
        EXPECT_EQ(rt, 0);

        rt = my_zk.CheckPathExist(test_node);
        EXPECT_EQ(rt, -1);
    }
}

/// test: create node force
TEST(TestZookeeperCommon, TestCreateNodeForce) {
    ZookeeperCommon my_zk;
    std::string value;
    int rt = my_zk.Init(FLAGS_cluster_name, FLAGS_zk_servers);
    if (rt < 0) {
        LOG(ERROR) << "connect zk failed.";
    } else {
        EXPECT_EQ(rt, 0);
        /// create a normal node, the result will be 0
        rt = my_zk.CreateNode(test_node);
        EXPECT_EQ(rt, 0);
        rt = my_zk.CheckPathExist(test_node);
        EXPECT_EQ(rt, 0);
        /// if you create a node already exists, the result is also 0
        rt = my_zk.CreateNode(test_node);
        EXPECT_EQ(rt, 0);
        rt = my_zk.CheckPathExist(test_node);
        EXPECT_EQ(rt, 0);
        /// if you create a node forcelly which has no parent, the result is 0
        rt = my_zk.CreateNodeForce(test_node + "/no_parent/node");
        EXPECT_EQ(rt, 0);
        rt = my_zk.CheckPathExist(test_node + "/no_parent/node");
        EXPECT_EQ(rt, 0);
        /// create a even deep node
        rt = my_zk.CreateNodeForce(test_node + "/no_parent/node/node/node");
        EXPECT_EQ(rt, 0);
        rt = my_zk.CheckPathExist(test_node + "/no_parent/node/node/node");
        EXPECT_EQ(rt, 0);
        /// delete the node that created just now
        rt = my_zk.DeleteNodeForceRecursively(test_node);
        EXPECT_EQ(rt, 0);
        rt = my_zk.CheckPathExist(test_node);
        EXPECT_EQ(rt, -1);
    }
}

/// test: get all children of a node
TEST(TestZookeeperCommon, TestGetChildren) {
    ZookeeperCommon my_zk;
    std::string value;
    int rt = my_zk.Init(FLAGS_cluster_name, FLAGS_zk_servers);
    if (rt < 0) {
        LOG(ERROR) << "connect zk failed.";
    } else {
        EXPECT_EQ(rt, 0);
        std::map<std::string, std::string> children;
        my_zk.CreateNodeForce(test_node + "/job_0");
        my_zk.CreateNodeForce(test_node + "/job_1");
        rt = my_zk.GetChildren(test_node, &children);
        PrintMap(children);
        EXPECT_EQ(rt, 2);
        EXPECT_EQ(static_cast<int>(children.size()), 2);
        rt = my_zk.GetChildren(test_node + "/unknown_mavisluo", &children);
        EXPECT_EQ(rt, -1);
        /// delete the test nodes
        my_zk.DeleteNode(test_node + "/job_0");
        my_zk.DeleteNode(test_node + "/job_1");
        my_zk.DeleteNode(test_node);
    }
}

/// test: w check path exist
TEST(TestZookeeperCommon, TestWCheckPathExist) {
    ZookeeperCommon my_zk;
    std::string value;
    int rt = my_zk.Init(FLAGS_cluster_name, FLAGS_zk_servers);
    if (rt < 0) {
        LOG(ERROR) << "connect zk failed.";
    } else {
        EXPECT_EQ(rt, 0);
        std::map<std::string, std::string> children;
        rt = my_zk.CreateNodeForce(test_node + "/second_node");
        EXPECT_EQ(rt, 0);
        rt = my_zk.WCheckPathExist(test_node + "/second_node", &Watcher);
        EXPECT_EQ(rt, 0);

        rt = my_zk.SetValueOfNode(test_node + "/second_node", "new_value");
        rt = my_zk.SetValueOfNode(test_node + "/second_node", "new_value_2");
        my_zk.DeleteNode(test_node + "/second_node");
        my_zk.DeleteNode(test_node);
    }
}

/// test: w get value of a node
TEST(TestZookeeperCommon, TestWGetValueOfNode) {
    ZookeeperCommon my_zk;
    std::string value;
    int rt = my_zk.Init(FLAGS_cluster_name, FLAGS_zk_servers);
    if (rt < 0) {
        LOG(ERROR) << "connect zk failed.";
    } else {
        EXPECT_EQ(rt, 0);
        std::map<std::string, std::string> children;
        rt = my_zk.CreateNodeForce(test_node + "/second_node");
        EXPECT_EQ(rt, 0);
        rt = my_zk.SetValueOfNode(test_node + "/second_node", "first_value");

        rt = my_zk.WGetValueOfNode(test_node + "/second_node", &Watcher, &value);
        LOG(INFO) << "===========" << value;
        EXPECT_EQ(rt, 0);

        rt = my_zk.CheckPathExist(test_node + "/second_node");
        EXPECT_EQ(rt, 0);
        rt = my_zk.SetValueOfNode(test_node + "/second_node", "new_value");
        rt = my_zk.SetValueOfNode(test_node + "/second_node", "new_value_2");
        my_zk.DeleteNode(test_node + "/second_node");
        my_zk.DeleteNode(test_node);
    }
}

/////////////////////////////////////////
//// test for dynamic configuration
/////////////////////////////////////////

// define some reconfig func
void ReConfigReportScheddTimeout(std::string value) {
    int new_value = atoi(value.c_str());
    FLAGS_report_schedd_timeout = 3 * new_value + 1;
}

void ReConfigSchedulerHttpEndpoint(std::string value) {
    FLAGS_scheduler_http_endpoint = value;
}

void ReConfigHousekeeperCycle(std::string value) {
    FLAGS_housekeeper_cycle = atoi(value.c_str());
}

/// test TestCollectorConf
TEST(TestCollectorConf, TestDynamicConfig) {
    /// get zkcommon to change config item later
    ZookeeperCommon zk;
    int rt = zk.Init(FLAGS_cluster_name, FLAGS_zk_servers);
    if (rt < 0) {
        std::cout << "tns_test init failed." << std::endl;
        return;
    }
    /// dynamic configuration
    DynamicConfiguration dynamic_collector;
    if (!dynamic_collector.Init("collector")) {
        std::cout << "init error" << std::endl;
        return;
    }

    dynamic_collector.RegisterOneReconfigFun(
            "startd_update_interval",
            ReConfigReportScheddTimeout);
    dynamic_collector.RegisterOneReconfigFun(
            "housekeeper_cycle",
            ReConfigHousekeeperCycle);
    /// test global config item
    dynamic_collector.RegisterOneReconfigFun(
            "scheduler_http_endpoint",
            ReConfigSchedulerHttpEndpoint);
    /// set the watcher
    dynamic_collector.WatchAll();
    sleep(1);
    /// change the node of zk

    zk.SetValueOfNode("/zk/xaec/tborg/" + FLAGS_cluster_name + \
            "/config/global/startd_update_interval", "10");
    zk.SetValueOfNode("/zk/xaec/tborg/" + FLAGS_cluster_name + \
            "/config/global/scheduler_http_endpoint", "10.6.207.16:9688");
    zk.SetValueOfNode("/zk/xaec/tborg/" + FLAGS_cluster_name + \
            "/config/collector/housekeeper_cycle", "2000");
    int i = 0;
    while (i < 3) {
        sleep(1);
        ++i;
    }

    /// check if the configuration has changed successfully
    EXPECT_EQ(FLAGS_report_schedd_timeout, 31);
    EXPECT_EQ(FLAGS_scheduler_http_endpoint, "10.6.207.16:9688");
    EXPECT_EQ(FLAGS_housekeeper_cycle, 2000);
}

int main(int argc, char *argv[]) {
    testing::AddGlobalTestEnvironment(new MyEnvironment);
    testing::InitGoogleTest(&argc, argv);
    if (google::ParseCommandLineFlags(&argc, &argv, true)) {
        for (int i = 0; i < argc; ++i) {
            printf("arg[%d]=%s\n", i, argv[i]);
        }
    } else {
        return  EXIT_FAILURE;
    }
    FLAGS_cluster_name = "xaec-comprehensive_test_zk_" + get_hostip();
    test_node = "/zk/xaec/tborg/" + FLAGS_cluster_name + \
                "/tns/mavisluo";

    return RUN_ALL_TESTS();
}
