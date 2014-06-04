/********************************
 FileName: common/clynn/thread.h
 Author:   WangMin
 Date:     2013-09-24
 Version:  0.1
 Description: thread, use a thread to start a func
*********************************/

#ifndef SRC_COMMON_THREAD_H
#define SRC_COMMON_THREAD_H

#include <pthread.h>

#include <tr1/memory>
#include <tr1/functional>

using std::tr1::function;
using std::tr1::placeholders::_1;
using std::tr1::placeholders::_2;

namespace clynn
{
    class Thread
    {
    public:
        typedef function<void()> ThreadFunc;
   
    public:
        Thread() : m_entry(NULL),
                   m_context(NULL),
                   m_param(0),
                   m_is_running(false) {}

        Thread(ThreadFunc entry, void* context = NULL, unsigned long long param = 0) : m_entry(entry), 
       m_context(context),
       m_param(param),
       m_is_running(false) {}

        virtual ~Thread() {}

        //void Init(ThreadFunc entry, void* context = NULL, unsigned long long param = 0);
        void DoStart(){
            m_is_running = true;
            m_entry();
        }

        bool Start() {
            int ret = pthread_create(&m_id, NULL, Entry, (void*)this);
            return ret == 0;
        }

        bool Join(){
            return pthread_join(m_id, NULL) == 0;
        }

        bool IsRunning() {
            return m_is_running;
        }

        void Terminate(){
            m_is_running = false;
        }

    private:
        static void* Entry(void* in_thread){
            Thread* th = (Thread* )in_thread;
            th->DoStart();
            th->Terminate();
            return NULL;
        }
    private:
        pthread_t m_id;
        ThreadFunc m_entry;
        void* m_context;
        unsigned long long m_param;
        bool m_is_running;
    }; 
}

#endif
