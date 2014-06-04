#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<string>
#include<iostream>

using std::string;
using std::cout;
using std::endl;

int main(){
    string process_name = "init";
    FILE* fp;
    char buf[256];
    string cmd;
    int count;
    cmd = "ps -C " + process_name  + " |wc -l";
    if((fp=popen(cmd.c_str(), "r")) == NULL) {
        cout<<"popen error"<<endl;
        return -1;
    }
    if((fgets(buf, 128, fp))!= NULL ) {
        count = atoi(buf);
        if(count-1 == 1) {
	    cout<<"yes"<<endl;
	    return 0;
        } else {
            std::cout<<"process not find"<<std::endl;
            return -1;
        }
    } else {
        std::cout<<"get process num from file  error"<<std::endl;
        return -1;
    }
}
