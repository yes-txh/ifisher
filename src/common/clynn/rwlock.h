/********************************
 FileName: common/clynn/rwlock.h
 Author:   
 Date:     2013-08-21
 Version:  0.1
 Description: the class about pthread_rwlock_t, lock of read and write
*********************************/

#ifndef COMMON_RWLOCK_H
#define COMMON_RWLOCK_H

#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <string>
#include <stdexcept>

using std::string;
using std::runtime_error;

namespace clynn {

    class RWLock {
        public:
            RWLock() {
                CheckError("Rwlock::Rwlock", pthread_rwlock_init(&m_lock, NULL));
            }

            ~RWLock() {
                pthread_rwlock_destroy(&m_lock);
            }

            void ReadLock() {
                CheckError("RWLock::ReadLock", pthread_rwlock_rdlock(&m_lock));
            }

            void WriteLock() {
                CheckError("RWLock::WriteLock", pthread_rwlock_wrlock(&m_lock));
            }

            void Unlock() {
                CheckError("RWLock::Unlock", pthread_rwlock_unlock(&m_lock));
            }

        private:
            void CheckError(const char* info, int code) {
                if (code != 0) {
                    string msg = info;
                    msg += " error: ";
                    msg += strerror(code);
                    throw runtime_error(msg);
                }
            }

        private:
            pthread_rwlock_t m_lock;
    };

    class ReadLocker {
        public:
            explicit ReadLocker(RWLock& rwlock) : m_rwlock(&rwlock) {
                m_rwlock->ReadLock();
            }

            ~ReadLocker() {
                m_rwlock->Unlock();
            }

            void Unlock() {
                m_rwlock->Unlock();
            }

            void Lock() {
                m_rwlock->ReadLock();
            }

        private:
            RWLock* m_rwlock;
    };

    class WriteLocker {
        public:
            explicit WriteLocker(RWLock& rwlock) : m_rwlock(&rwlock) {
                m_rwlock->WriteLock();
            }

            ~WriteLocker() {
                m_rwlock->Unlock();
            }

            void Unlock() {
                m_rwlock->Unlock();
            }
            
            void Lock() {
                m_rwlock->WriteLock();
            }

        private:
            RWLock* m_rwlock;
    };
}
#endif
