/********************************
 FileName: executor/system.h
 Author:   WangMin
 Date:     2013-08-21
 Version:  0.1
 Description: get the physical resource info of the machine
*********************************/

#ifndef SRC_EXECUTOR_SYSTEM_H
#define SRC_EXECUTOR_SYSTEM_H

#include <iostream>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <unistd.h>
// #include <netdb.h>
#include <string>
#include <errno.h>
#include <stdio.h>

using std::string;

class System {
public:
    /// @brief: Load 
    // get cpu average load(1 min)
    static double GetLoadAvginOne();

    // get cpu average load(5 min)
    static double GetLoadAvginFive();

    // get cpu average load(15 min)
    static double GetLoadAvginFifteen();

    /// @brief: CPU
    // get cpu logic core number
    static int32_t GetCpuNum();

    // get total cpu time in USER_HZ unit
    static uint64_t GetCpuTime();

    // get cpu usage
    static double GetCpuUsage();


    /// @brief: Memory
    // get total physical memroy size(MB)
    static int32_t GetTotalMemory();

    // get free physical memory size(MB)
    static int32_t GetPhysicalMemory();

    // get used memory size(MB)
    static int32_t GetUsedMemory();

    // get memory usage
    static double GetMemoryUsage();

    // get swaptotal memory size(MB)
    static int32_t GetSwapTotalMemory();

    // get swapfree memory size(MB)
    static int32_t GetSwapFreeMemory();


    /// @brief: network flow

    // get ip by interface
    static int32_t GetIP(const char* interface, char* ip);

    static string GetIP(const char* interface);

    // get net flow(size: Bytes)
    static void GetNetFlowinBytes(const char* interface, int32_t& bytes_in, int32_t& bytes_out);

    // get net flow(size: bits)
    static void GetNetFlowinbits(const char* interface, int32_t& bits_in, int32_t& bits_out);

    // get static net bandwidth, mbits/s
    static int32_t GetBandWidth(const char* interface);

    // get if type
    static string GetNICType(const char* interface);

    /// @brief: Disk
    // get total disk size(GB)
    static int32_t GetTotalDisk();

    // get used disk(GB)
    static int32_t GetUsedDisk();

    // get disk usage
    static double GetDiskUsage();

    // for img
    static int32_t GetFakeSpace();

    /// @brief: others
    // get os version
    static string GetOSVersion();

    // remove the directory
    static void RemoveDir(const char* path);

    // get current time
    static void GetCurrentTime(char* str, int32_t len);

};

#endif
