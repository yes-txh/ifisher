/********************************
 FileName: common/clynn/cond.h
 Author:   
 Date:     2013-08-21
 Version:  0.1
 Description: pthread condition lock 
*********************************/

#ifndef COMMON_COND_H
#define COMMON_COND_H

#include <pthread.h>

#include <string>
#include "common/clynn/mutex.h"

using std::runtime_error;
using std::string;

namespace clynn {

    class Cond {
        public:
            Cond() {
                CheckError("Cond::Cond", pthread_cond_init(&m_cond, NULL));
            }
        
            ~Cond() { 
                pthread_cond_destroy(&m_cond);
            }

            /// send signal to wake up the thread
            void Signal(){
                CheckError("Cond::Signal", pthread_cond_signal(&m_cond));
            }

            /// block the thread, waitting to be woken up
            void Wait(Mutex& mutex){
                CheckError("Cond::Wait", pthread_cond_wait(&m_cond, &(mutex.m_lock)));
            }

            /// block the thread with timeout
            int Wait(Mutex& mutex, size_t timeout){
                struct timespec time;
                time.tv_sec = timeout;
                time.tv_nsec = 0;
                int ret = pthread_cond_timedwait(&m_cond, &(mutex.m_lock), &time);
                CheckError("Cond::TimeWait", ret);
                return ret;
            }

        private:
            void CheckError(const char* info, int code){
                if (code != 0) {
                    string msg = info;
                    msg += " error: ";
                    msg += strerror(code);
                    throw runtime_error(msg);
                }
            }
            pthread_cond_t m_cond;
    };
}

#endif
