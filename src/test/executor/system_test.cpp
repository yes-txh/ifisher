/********************************
 FileName: test/executor/system_test.cpp
 Author:   WangMin
 Date:     2013-08-21
 Version:  0.1
 Description: get the physical resource info of the machine, test
*********************************/
#include <iostream>
#include <string>

#include <gtest/gtest.h>
#include "executor/system.h"

using std::string;
using std::cout;
using std::endl;

int main(int argc, char** argv) {
    //tesing::InitGoogleTest(&argc, argv);
    // load
    printf("LoadAvginOne: %lf\n", System::GetLoadAvginOne());
    printf("LoadAvginFive: %lf\n", System::GetLoadAvginFive());
    printf("LoadAvginFifteen: %lf\n", System::GetLoadAvginFifteen());

    // cpu
    printf("CpuNum: %d\n", System::GetCpuNum());
    printf("CpuTime: %ld\n", System::GetCpuTime());
    printf("CpuUsage: %lf\n", System::GetCpuUsage());
    sleep(1);
    printf("CpuUsage: %lf\n", System::GetCpuUsage());

    // Memory
    printf("TotalMemory: %d\n", System::GetTotalMemory());
    printf("FreePhysicalMemory: %d\n", System::GetPhysicalMemory());
    printf("UsedMemory: %d\n", System::GetUsedMemory());
    printf("MemoryUsage: %lf\n", System::GetMemoryUsage());
    printf("SwapTotalMemory: %d\n", System::GetSwapTotalMemory());
    printf("SwapFreeMemory: %d\n", System::GetSwapFreeMemory());
   
    // net flow
    string interface = "eth0";
    int32_t in, out;
    System::GetNetFlowinBytes(interface.c_str(), in ,out);
    printf("IN: %d, OUT: %d\n", in , out);
    System::GetNetFlowinbits(interface.c_str(), in ,out);
    printf("IN: %d, OUT: %d\n", in , out);
    printf("Bandwidth: %dMb/s\n", System::GetBandWidth(interface.c_str()));
    cout << "NICType: " << System::GetNICType(interface.c_str()) << endl;

    // disk
    printf("TotalDisk: %d\n", System::GetTotalDisk());
    printf("UsedDisk: %d\n", System::GetUsedDisk());
    printf("DiskUsage: %f\n", System::GetDiskUsage());
 
    // others
    cout << System::GetOSVersion() << endl;

    System::RemoveDir("/tmp/cellos");
 
    char* curTime;
    curTime = new char[30];
    System::GetCurrentTime(curTime, 30);
    printf("GetCurTime: %s\n", curTime);
    delete []curTime;

    return 0;
}
