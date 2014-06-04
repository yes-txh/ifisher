/********************************
 FileName: common/clynn/block_queue.h
 Author:   
 Date:     2013-08-21
 Version:  0.1
 Description: block queue with list and pthread_cond_t
*********************************/
 
#ifndef COMMON_BLOCK_QUEUE_H
#define COMMON_BLOCK_QUEUE_H

#include <list>

#include "common/clynn/cond.h"
#include "common/clynn/mutex.h"

using std::list;
using clynn::Mutex;
using clynn::Cond;
using clynn::MutexLocker;

namespace clynn {

    template <typename T>
    class BlockQueue {
        public:
            BlockQueue() {
                m_empty = new Cond;
                m_mutex = new Mutex;
            }

            ~BlockQueue() {
                delete m_empty;
                delete m_mutex;
            }

            void PushBack(const T& element) {
                MutexLocker locker(*m_mutex);
                m_queue.push_back(element);
                if (Size() == 1)
                    m_empty->Signal();
            }

            void PopFront(T* value) {
                MutexLocker locker(*m_mutex);
                if (Size() <= 0)
                    m_empty->Wait(*m_mutex);
                *value = m_queue.front();
                m_queue.pop_front();
            }

            bool PopFront(T* value, size_t timeout) {
                MutexLocker locker(*m_mutex);
                if (Size() <= 0) {
                    if (m_empty->Wait(*m_mutex, timeout) == 0) {
                        *value = m_queue.front();
                        m_queue.pop_front();
                        return true;
                    } else {
                        return false;
                    }
                }
                *value = m_queue.front();
                m_queue.pop_front();
                return true;
            }

            int Size() {
                return m_queue.size();
            }
           
            bool IsEmpty() {
                return m_queue.empty();
            }
            
            void Wait() {
                MutexLocker locker(*m_mutex);
                if (Size() <= 0)
                    m_empty->Wait(*m_mutex);
            }
 
        private:
            Cond* m_empty;
            Mutex* m_mutex;
            // block queue by a list
            list<T> m_queue;
    };

}

#endif
