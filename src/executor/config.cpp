/********************************
 FileName: executor/config.cpp
 Author:   WangMin
 Date:     2013-08-20
 Version:  0.1
 Description: config for executor
*********************************/

#include "gflags/gflags.h"

DEFINE_int32(port, 9997, "executor port");
DEFINE_string(resource_scheduler_endpoint, "127.0.0.1:9998", "ResouceScheduler endpoint");
DEFINE_string(jobs_manager_endpoint, "127.0.0.1:9999", "JobsManager endpoint");
DEFINE_int32(hb_interval, 15, "heartbeat interval");
DEFINE_int32(vm_hb_interval, 15, "vm heartbeat interval");
DEFINE_string(interface, "br0", "network interface, communicate with master");
DEFINE_string(if_bridge, "br0", "network interface, communicate with vms");
DEFINE_string(log_path, "../log/", "executor log path");
DEFINE_string(libvirt_dir, "/var/lib/libvirt/images/", "libvirt work dir");
DEFINE_string(xml_template, "../conf/kvm.xml", "kvm xml template");
DEFINE_string(lxc_dir, "/var/lib/lynn/lxc/", "lxc work dir");
DEFINE_string(lxc_template, "/var/lib/lynn/lxc/", "default lxc conf template");
DEFINE_string(hdfs_host, "localhost", "hdfs local host name, uname -n");
DEFINE_int32(hdfs_port, 9000, "hdfs default port");
DEFINE_bool(lxc_create, true, "include lxc-create -n 'lxc-name'");
DEFINE_bool(lxc_relative_path, true, "lxc relative path");
DEFINE_string(image_path, "/var/lib/lynn/kvm/", "executor image path");

DEFINE_int32(starting_timeout, 5, "task starting timeout");
DEFINE_string(lynn_version, "", "executor version");

// ip pool
DEFINE_string(ip_range, "192.168.10.80-192.168.10.90", "range of ip addrs");

// debug
DEFINE_bool(debug, false, "print some debug information");
DEFINE_bool(jobs_manager_up, true, "JobsManager up");
DEFINE_bool(resource_scheduler_up, true, "ResourceScheduler up");

// DEFINE_string(work_directory, "/tmp/cello", "cellet work directory");
// DEFINE_string(dfs_ip, "", "distributed file system server ip");
// DEFINE_int32(dfs_port, 0, "distributed file system server port");
// DEFINE_string(policy_file, "", "policy configuration file");
