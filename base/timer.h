/*!
 *  Copyright (c) 2016 by Contributors
 *  \file timer.h
 *  \author ZhouYong
 */
#ifndef TOOLS_TIMER_H_
#define TOOLS_TIMER_H_

#include <sys/time.h>
#include <time.h>
#include <memory>
#include <string>
#include <unordered_map>
#include "system_info.h"

namespace openmi {

/*! \brief return time in second */
inline double GetTime(void) {
  return static_cast<double>(time(NULL)); 
  // TODO clock_gettime only support linux/unix platform compile
  /*
  timespec ts;
  std::string os_name = GetOsName();
  if (os_name != "Darwin" && clock_gettime(CLOCK_REALTIME, &ts) == 0) {
    return static_cast<double>(ts.tv_sec) + 
      static_cast<double>(ts.tv_nsec) * 1e-9;
  } else {
    return static_cast<double>(time(NULL));
  }
  */
}

/*! \brief return timestamp */
inline unsigned long TimeStamp(void) {
  return static_cast<unsigned long>(time(NULL));
} 

inline uint64_t CurrentTimeUs() {
  struct timeval timestamp;
  gettimeofday(&timestamp, NULL);
  return timestamp.tv_sec * 1000000UL + timestamp.tv_usec;
}

/*!
 * \brief Timer class used to stat timer overhead. time unit: us
 */
class Timer {
public:
  Timer() { gettimeofday(&start_, NULL); }

  inline int Elapsed(bool restart = true) {
    gettimeofday(&end_, NULL);
    int t = (end_.tv_sec - start_.tv_sec) * 1000 * 1000 + (end_.tv_usec - start_.tv_usec);
    if (restart) start_ = end_;
    return t;
  }

  inline int GetElapsed() {
    return (end_.tv_sec - start_.tv_sec) * 1000000UL + (end_.tv_usec - start_.tv_usec);
  }

  inline void EndTimer() { gettimeofday(&end_, NULL); }

  inline void Restart() { gettimeofday(&start_, NULL); }

  inline long CurTimeStamp() { return start_.tv_sec; }

private:
  timeval start_;
  timeval end_;
}; // class Timer
typedef std::shared_ptr<Timer> TimerPtr;

/*!
 * \brief TimerManager
 */
class TimerManager {
public:
  TimerManager() {}

  ~TimerManager() { timer_map_.clear(); }

  TimerPtr AddTimer(const std::string& name) {
    if (timer_map_.find(name) != timer_map_.end())  {
      return timer_map_[name];
    }
    TimerPtr timer = std::make_shared<Timer>();
    timer_map_[name] = timer;
    return timer;
  }

  void EndTimer(const std::string& name) {
    if (timer_map_.find(name) == timer_map_.end()) {
      //LOG(ERROR) << "name not exists in timer_map_. name[" << name << "]";
      return;
    }
    timer_map_[name]->EndTimer();
  }

  int GetElapsed(const std::string& name) {
    if (timer_map_.find(name) == timer_map_.end()) {
      //LOG(ERROR) << name << "name not exists in timer_map_. name[" << name << "]";
      return -1;
    }
    return timer_map_[name]->GetElapsed();
  }

  TimerPtr GetTimer(const std::string& name) {
    if (timer_map_.find(name) == timer_map_.end()) {
      return nullptr;
    }
    return timer_map_[name];
  }

private:
  std::unordered_map<std::string, TimerPtr> timer_map_;
}; // class TimerManager

typedef std::shared_ptr<TimerManager> TimerManagerPtr;

} // namespace
#endif // TOOLS_TIMER_H_
