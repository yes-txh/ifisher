/********************************
 FileName: executor/system.h
 Author:   WangMin
 Date:     2013-08-21
 Version:  0.1
 Description: get the physical resource info of the machine
*********************************/

#include <unistd.h>
#include <sys/sysinfo.h>
#include <sys/statfs.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <fcntl.h>
#include <time.h>
#include <sys/param.h>
#include <dirent.h>

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <algorithm>
#include <boost/algorithm/string.hpp>

#include "executor/system.h"

using boost::trim;

#define __STDC_FORMAT_MACROS
#include <inttypes.h> // for scanf uint64_t

/// @brief: Load
// @return: return avg load in 1 minute if success, or return -1
double System::GetLoadAvginOne() {
    // fs /proc
    FILE* fp = fopen("/proc/loadavg", "r");
    if (!fp)
        return -1.0;

    // get info from /proc/loadavg 
    double avg_1, avg_5, avg_15;
    fscanf(fp, "%lf %lf %lf", &avg_1, &avg_5, &avg_15);

    // close file
    fclose(fp);
    return avg_1;
}

// @return: return avg load in 5 minutes if success, or return -1
double System::GetLoadAvginFive() {
    // fs /proc
    FILE* fp = fopen("/proc/loadavg", "r");
    if (!fp)
        return -1.0;

    // get info from /proc/loadavg 
    double avg_1, avg_5, avg_15;
    fscanf(fp, "%lf %lf %lf", &avg_1, &avg_5, &avg_15);

    // close file
    fclose(fp);
    return avg_5;
}

// @return: return avg load in 15 minutes if success, or return -1
double System::GetLoadAvginFifteen() {
    // fs /proc
    FILE* fp = fopen("/proc/loadavg", "r");
    if (!fp)
        return -1.0;

    // get infoi from /proc/loadavg 
    double avg_1, avg_5, avg_15;
    fscanf(fp, "%lf %lf %lf", &avg_1, &avg_5, &avg_15);

    // close file
    fclose(fp);
    return avg_15;
}

/// @brief: CPU
// @return: logic cpu numbers
int32_t System::GetCpuNum() {
    // fs /proc
    FILE* fp = fopen("/proc/cpuinfo", "r");
    if (!fp)
        return 1;

    int32_t cpu_num = 0;
    char buf[256] = {0};
    // read each line, count cpu number as line number
    while (fgets(buf, sizeof(buf)-1, fp)) {
        if (strncmp(buf, "processor", 9) == 0) {
            cpu_num++;
        }
    }

    // close file
    fclose(fp);
    return cpu_num ? cpu_num : 1;
}

// @return: cpu time
uint64_t System::GetCpuTime() {
    // fs /porc
    FILE* fp = fopen("/proc/stat", "r");
    if (!fp)
         return 1;

    uint64_t cpu_user = 0;
    uint64_t cpu_nice = 0;
    uint64_t cpu_sys = 0;
    uint64_t cpu_idle = 0;
    uint64_t cpu_iowait = 0;
    uint64_t cpu_hardirq = 0;
    uint64_t cpu_softirq = 0;
    uint64_t total = 0;
    char line[8192];
    // get cpu time
    while (fgets(line, 8192, fp) != NULL) {
        if (!strncmp(line, "cpu ", 4)) {
            //sscanf(line + 5, "%"PRIu64" %"PRIu64" %"PRIu64" %"PRIu64" %"PRIu64" %"PRIu64" %"PRIu64"",
	    sscanf(line + 5, "%llu %llu %llu %llu %llu %llu %llu",
                    &cpu_user, &cpu_nice, &cpu_sys, &cpu_idle,
                    &cpu_iowait, &cpu_hardirq, &cpu_softirq);
            total = cpu_user + cpu_nice + cpu_sys +
                   cpu_iowait + cpu_hardirq + cpu_softirq + cpu_idle;
            break;
        }
    }

    // close file
    fclose(fp);
    return total;
}

// @return: cpu usage
double System::GetCpuUsage() {
    FILE* fp = fopen("/proc/stat", "r");
    if (!fp)
        return -1.0;

    char line[8192];
    // previous cpu time 
    static uint64_t prev_used = 0;
    static uint64_t prev_total = 0;
    static bool first = true;

    // cpu time now
    uint64_t cpu_user = 0;
    uint64_t cpu_nice = 0;
    uint64_t cpu_sys = 0;
    uint64_t cpu_idle = 0;
    uint64_t cpu_iowait = 0;
    uint64_t cpu_hardirq = 0;
    uint64_t cpu_softirq = 0;
    uint64_t used = 0;
    uint64_t total = 0;

    while (fgets(line, 8192, fp) != NULL) {
        if (!strncmp(line, "cpu ", 4)) {
            //sscanf(line + 5, "%"PRIu64" %"PRIu64" %"PRIu64" %"PRIu64" %"PRIu64" %"PRIu64" %"PRIu64"",
	    sscanf(line + 5, "%llu %llu %llu %llu %llu %llu %llu",
                   &cpu_user, &cpu_nice, &cpu_sys, &cpu_idle,
                   &cpu_iowait, &cpu_hardirq, &cpu_softirq);
            used = cpu_user + cpu_nice + cpu_sys +
                   cpu_iowait + cpu_hardirq + cpu_softirq;
            total = used + cpu_idle;
            break;
        }
    }

    if (first) {
        first = false;
        prev_used = used;
        prev_total = total;
        fclose(fp);
        return 0.0;
    }

    // avoid 0
    if (total == prev_total) {
        total = prev_total + 1;
    }
    double cpu_usage = static_cast<double>(used - prev_used)
                       / static_cast<double>(total - prev_total);
    prev_used = used;
    prev_total = total;

    // close file
    fclose(fp);
    return cpu_usage;
}

/// @brief: Memory
// @return: total physical memory, size(MB)
int32_t System::GetTotalMemory() {
    // fs /proc
    FILE* fp = fopen("/proc/meminfo", "r");
    if (!fp)
        return -1;

    int32_t total = 0;
    char buf[1024];
    // get memory info
    while (fgets(buf, sizeof(buf)-1, fp)) {
        if (strncmp(buf, "MemTotal:", 9) != 0)
            continue;
        char* b = strstr(buf, "MemTotal:");
        sscanf(b, "MemTotal: %d", &total);
    }

    // close file
    fclose(fp);
    total /= 1024;
    return total;
}    
   
// @return: free physical memoryi, size(MB)
int32_t System::GetPhysicalMemory() {
   // fs /proc
   FILE* fp = fopen("/proc/meminfo", "r");
   if (!fp)
       return -1;

   int32_t free = 0;
   int32_t buffer = 0;
   int32_t cache = 0;
   char buf[1024];
   // get MemFree Buffers Cached from /proc/meminfo
   while (fgets(buf, sizeof(buf)-1, fp)) {
       if (0 == strncmp(buf, "MemFree:", 8)) {
           char* b = strstr(buf, "MemFree: ");
           sscanf(b, "MemFree: %d", &free);
           continue;
       } else if (0 == strncmp(buf, "Buffers:", 8)) {
           char* b = strstr(buf, "Buffers: ");
           sscanf(b, "Buffers: %d", &buffer);
           continue;
       } else if (0 == strncmp(buf, "Cached:", 7)) {
           char* b = strstr(buf, "Cached: ");
           sscanf(b, "Cached: %d", &cache);
           break;
       }
   }
   
   // close file
   fclose(fp);
   return (free + buffer + cache) / 1024;
}

// @return: used memory size(MB)
int32_t System::GetUsedMemory() {
    int32_t total = GetTotalMemory();
    int32_t free = GetPhysicalMemory();

    if (total == -1 || free == -1 || free > total)
        return -1;
    return total - free;
}

// @return usage of memory
double System::GetMemoryUsage() {
    int32_t total = GetTotalMemory();
    int32_t used = GetUsedMemory();

    if (total == -1 || used == -1 || used > total)
        return -1;
    return (double)used / total;
}

// @return: return SwapTotal memory(size:MB) if success, or return -1
int32_t System::GetSwapTotalMemory() {
    // fs /proc
    FILE* fp = fopen("/proc/meminfo", "r");
    if (!fp)
        return -1;

    int32_t swap_total = -1;
    char buf[256] = {0};
    // get SwapTotal from /proc/meminfo
    while (fgets(buf, sizeof(buf) - 1, fp)) {
        if (0 == strncmp(buf, "SwapTotal", 9)) {
            if (sscanf(buf, "SwapTotal: %d", &swap_total) < 1) {
                fclose(fp);
                return -1;
            }
            break;
        }
    }
    
    // close file
    fclose(fp);
    swap_total /= 1024;
    return swap_total;
}

// @return: return SwapFree memory(size:MB) if success, or return -1
int32_t System::GetSwapFreeMemory() {
    // fs /proc
    FILE* fp = fopen("/proc/meminfo", "r");
    if (!fp)
        return -1;

    int32_t swap_free = -1;
    char buf[256] = {0};
    // get SwapFree from /proc/meminfo
    while (fgets(buf, sizeof(buf) - 1, fp)) {
        if (0 == strncmp(buf, "SwapFree", 8)) {
            if (sscanf(buf, "SwapFree: %d", &swap_free) < 1) {
                fclose(fp);
                return -1;
            }
            break;
        }
    }
   
    // close file
    fclose(fp);
    swap_free /= 1024;
    return swap_free;
}

/// @brief: network flow

// get ip by interface
int32_t System::GetIP(const char* interface, char* ip) {
    int32_t sock;
    struct sockaddr_in sin;
    struct ifreq ifr;
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("socket");
        return -1;
    }

    strncpy(ifr.ifr_name, interface, IFNAMSIZ);
    ifr.ifr_name[IFNAMSIZ - 1] = 0;
    if (ioctl(sock, SIOCGIFADDR, &ifr) < 0) {
        perror("ioctl");
        return -1;
    }

    memcpy(&sin, &ifr.ifr_addr, sizeof(sin));
    char * tmp = inet_ntoa(sin.sin_addr);
    strncpy(ip, tmp, strlen(tmp));
    return 0;
}

string System::GetIP(const char* interface) {
    char ip[16] = {0};
    GetIP(interface, ip);
    return ip;
}

// get bytes_in and bytes_out of a interface
void System::GetNetFlowinBytes(const char* interface, int32_t& bytes_in, int32_t& bytes_out) {
    // cleared 0
    bytes_in = bytes_out = 0;

    // fs /proc
    FILE *fp = fopen("/proc/net/dev", "r");
    if(!fp)
        return;

    char buf[256];
    char if_name[30];
    long rb, rpkt, r_err, r_drop, r_fifo, r_frame, r_compr, r_mcast;
    long tb, tpkg, t_err, t_drop, t_fifo, t_frame, t_compr, t_mcast;

    // read interface into if_name
    snprintf(if_name, 30, "%s:", interface);

    // read bytes from /proc/net/dev
    while (fgets(buf, sizeof(buf) - 1, fp)) {
        char* p = buf;
        // remove space(qu diao kong ge)
        while (' ' == *p)
            p++;

        if (!strncmp(p, if_name, strlen(if_name))) {
            // 
            char* b = strstr(p, if_name);
            sscanf(b + strlen(if_name), "%ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld", &rb, &rpkt, &r_err, &r_drop, &r_fifo, &r_frame, &r_compr, &r_mcast, &tb, &tpkg, &t_err, &t_drop, &t_fifo, &t_frame, &t_compr, &t_mcast);
            bytes_in = rb;
            bytes_out = tb;
        }
    }
    // close file
    fclose(fp);
}

void System::GetNetFlowinbits(const char* interface, int32_t& bits_in, int32_t& bits_out) {
    // cleared 0
    bits_in = bits_out = 0;

    // fs /proc
    FILE *fp = fopen("/proc/net/dev", "r");
    if (!fp)
        return;

    char buf[256];
    char if_name[30];
    long rb, rpkt, r_err, r_drop, r_fifo, r_frame, r_compr, r_mcast;
    long tb, tpkg, t_err, t_drop, t_fifo, t_frame, t_compr, t_mcast;

    // read interface into if_name
    snprintf(if_name, 30, "%s:", interface);

    // read bytes from /proc/net/dev
    while (fgets(buf, sizeof(buf) - 1, fp)) {
        char* p = buf;
        // remove space(qu diao kong ge)
        while (' ' == *p)
            p++;

        if (!strncmp(p, if_name, strlen(if_name))) {
            // 
            char* b = strstr(p, if_name);
            sscanf(b + strlen(if_name), "%ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld", &rb, &rpkt, &r_err, &r_drop, &r_fifo, &r_frame, &r_compr, &r_mcast, &tb, &tpkg, &t_err, &t_drop, &t_fifo, &t_frame, &t_compr, &t_mcast);
            bits_in = rb * 8;
            bits_out = tb * 8;
        }
    }
    // close file
    fclose(fp);
}

// get static net bandwidth
int32_t System::GetBandWidth(const char* interface) {
    char cmd[30] = {0};
    // cmd "ethtool eth0"
    snprintf(cmd, 30, "ethtool %s:", interface);

    FILE *stream = popen(cmd, "r");
    if (NULL == stream)
        return -1;

    int32_t bandwidth = -1;
    char buf[256] = {0};
    string s_buf = "";
    // get Speed from ethtool
    while (fgets(buf, sizeof(buf) - 1, stream)) {
        s_buf = buf;
        trim(s_buf);
         
        if (0 == strncmp(s_buf.c_str(), "Speed:", 6)) {
            if (sscanf(s_buf.c_str(), "Speed: %dMb/s", &bandwidth) < 1) {
                fclose(stream);
                return -1;
            }
            break;
        }
    }
    
    // close stream
    pclose(stream);
    return bandwidth;
}

// get interface type
string System::GetNICType(const char* interface) {
    char cmd[40] = {0};
    // cmd: "ethtool -i eth0"
    snprintf(cmd, 40, "ethtool -i %s:", interface);

    FILE *stream = popen(cmd, "r");
    if (NULL == stream)
        return NULL;

    string nic_type = "a";
    char buf[256] = {0};
    char type[20] = {0};

    // get Speed from ethtool
    while (fgets(buf, sizeof(buf) - 1, stream)) {
        if (0 == strncmp(buf, "driver:", 7)) {
            if (sscanf(buf, "driver: %s", type) < 1) {
                fclose(stream);
                return NULL;
            }
            break;
        }
    }
    

    // close stream
    nic_type = type;
    pclose(stream);
    return nic_type; 
}

// @return total disk, size is G
int32_t System::GetTotalDisk() {
    // cmd 
    string cmd = "df --block-size=G";
    FILE *stream = popen(cmd.c_str(),"r");
    if (NULL == stream)
        return -1;

    char buf[1024];
    float percentage;
    char location[100], mount_on[100];
    int32_t capacity, used, available;
    // read disk info
    while (fgets(buf, sizeof(buf), stream)) {
        char* b = strstr(buf," /\n");
        if(b == NULL) {
            continue;
        }
        sscanf(buf, "%s %dG %dG %dG %f%% %s", location, &capacity, &used, &available, &percentage, mount_on);
    }

    // close stream
    pclose(stream);
    return capacity;
}

// @return used disk, size is G
int32_t System::GetUsedDisk() {
    // cmd 
    string cmd = "df --block-size=G";
    FILE *stream = popen(cmd.c_str(),"r");
    if (NULL == stream)
        return -1;

    char buf[1024];
    float percentage;
    char location[100], mount_on[100];
    int32_t capacity, used, available;
    // read disk info
    while (fgets(buf, sizeof(buf), stream)) {
        char* b = strstr(buf," /\n");
        if (b == NULL) {
            continue;
        }
        sscanf(buf, "%s %dG %dG %dG %f%% %s", location, &capacity, &used, &available, &percentage, mount_on);
    }

    // close stream
    pclose(stream);
    return used;
}

// @return: Disk usage
double System::GetDiskUsage() {
    int32_t total = System::GetTotalDisk();
    int32_t used = System::GetUsedDisk();

    if ((-1 == total) || (-1 == used) || (total < used))
        return -1;
    return (double)used/total;
}


/// @brief: others
// @return: Linux os version
string System::GetOSVersion() {
    // read cmd "uname -r"
    string cmd = "uname -r";
    FILE *stream = popen(cmd.c_str(),"r");
    if (NULL == stream)
        return NULL;

    // get version
    char buf[256];
    string version;
    fgets(buf, sizeof(buf), stream);

    // remove '\n'
    int32_t len = strlen(buf);
    buf[len-1] = '\0';
    version = buf;
    
    // close stream
    pclose(stream);
    return "Linux " + version;
}

// remove a dir, not a file
void System::RemoveDir(const char* path) {
    DIR* dp = opendir(path);
    if (dp) { 
        dirent* ep = NULL;
        while ((ep = readdir(dp)) != NULL) {
            // get rid of '..' and '.'
            if (strcmp("..", ep->d_name) != 0 &&
                strcmp(".", ep->d_name) != 0) {
                char file[256] = {0};
                snprintf(file, sizeof(file), "%s/%s", path, ep->d_name);
                struct stat info;
                stat(file, &info);
                // directory or file
                if (S_ISDIR(info.st_mode))
                    RemoveDir(file);
                else
                    unlink(file);
            }
        }
        rmdir(path);
    }
    
    // close dir
    closedir(dp);
}

// get current time
void System::GetCurrentTime(char* time_str, int32_t len)
{
    time_t cur_time;
    time(&cur_time);
    tm*p = gmtime(&cur_time);
    snprintf(time_str, len, "%d%02d%02d-%02d%02d%02d", (1900 + p->tm_year),
             (1 + p->tm_mon), p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
}
