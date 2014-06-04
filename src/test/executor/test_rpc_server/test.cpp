/********************************
 FileName: executor/executor.cpp
 Author:   WangMin
 Date:     2013-08-14
 Version:  0.1
 Description: executor main
*********************************/

#include <iostream>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include "common/clynn/rpc.h"
#include "service.h"

using std::string;
using std::cout;
using std::endl;
using std::auto_ptr;


// executor
int ExecutorEntity(int argc, char **argv) {
    int port = 9997; 
    RpcServer<TestService, TestProcessor>::Listen(port);
    //int port = 9999; 
    //Rpc<ExecutorService, ExecutorProcessor>::Listen(port);

    return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
    // is root?
    if (geteuid() != 0) {
        fprintf(stderr, "Executor: must be run as root, or sudo run it.\n");
        return EXIT_FAILURE;
    }

    ExecutorEntity(argc, argv);
    // monitor ExecutorEntity
    /* while(true) {
        int32_t status;
        int32_t pid = fork();
        if (pid != 0) {
            // parent process, start executorEntity when ExecutorEntity fail
            if (waitpid(pid, &status, 0) > 0) {
                continue;
            }
        } else {
            // child process
            ExecutorEntity(argc, argv);
        }
    }*/
    return EXIT_SUCCESS;
}
