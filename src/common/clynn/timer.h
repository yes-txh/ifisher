#ifndef COMMON_CLYNN_TIMER_H
#define COMMON_CLYNN_TIMER_H

#include <unistd.h>
#include <sys/time.h>

namespace clynn {

    class Timer {
    public:
       explicit Timer(bool auto_start = true) {
           m_is_running = auto_start; 
           m_cumulated_time = 0; 
           m_start_time.tv_sec = 0;
           m_start_time.tv_usec = 0;
           if(auto_start) {
               gettimeofday(&m_start_time, NULL); 
           }
       } 
  
       void Start() {
           if(m_is_running == false) { 
               m_is_running = true;
               gettimeofday(&m_start_time, NULL);
           } 
       }
  
       void Stop() {
           if(m_is_running == true) {
               m_is_running = false;
               struct timeval now;
               gettimeofday(&now, NULL); 
               m_cumulated_time += 
               1000000*(now.tv_sec - m_start_time.tv_sec) + now.tv_usec - m_start_time.tv_usec;
           }  
       }

       void Restart() {
           m_cumulated_time = 0; 
           gettimeofday(&m_start_time, NULL);
           m_is_running = true;
       }
      
       void Reset() {
           m_cumulated_time = 0;
           m_start_time.tv_sec = 0;
           m_start_time.tv_usec = 0;
           m_is_running = false;
       }
  
       int64_t ElapsedMicroSeconds() const {
           if(IsRunning()) {
                return m_cumulated_time + LastInterval();
           } else { 
                return m_cumulated_time;
           }
       }

       double ElapsedMilliSeconds() const {
           return ElapsedMicroSeconds() / 1000.0;
       }

       double ElapsedSeconds() const {
           return ElapsedMicroSeconds() / 1000000.0;
       }

       bool IsRunning() const {
           return m_is_running;
       }

    private:   
       int64_t LastInterval() const {
            struct timeval now;
            gettimeofday(&now, NULL);
            return 1000000 * (now.tv_sec - m_start_time.tv_sec) + now.tv_usec - m_start_time.tv_usec;
       }

       bool m_is_running;
       int64_t m_cumulated_time;
       struct timeval m_start_time;
    };
}
#endif
