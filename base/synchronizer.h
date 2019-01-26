/*!
 *  Copyright (c) 2017 by Contributors
 *  \file synchronizer.h
 *  \brief task/thread synchronizer
 */
#ifndef OPENMI_BASE_SYNCHRONIZER_H_
#define OPENMI_BASE_SYNCHRONIZER_H_

#include <condition_variable> // std::condition_variable 
#include <mutex>              // std::mutex, std::unique_lock
#include <string>

namespace openmi {

// Task/Thread Synchronizer
class Synchronizer {
public:
  Synchronizer(): task_id_(-1), task_name_("sync_task") {}
  
  Synchronizer(const int& task_id): task_id_(task_id), task_name_("sync_task") {}
  
  Synchronizer(const int& task_id, const std::string& task_name): task_id_(task_id), task_name_(task_name) {}
  
  Synchronizer(Synchronizer& sync) {
    task_id_ = sync.task_id_;
    task_name_ = sync.task_name_;
  }

  ~Synchronizer() {}

  inline int Init() {
    Clear();
    return 0;
  }
  
  void Wait(int total_num);
  
  void AddFinishOne(bool task_result); 
  
  void Clear();

  inline int GetTaskId() { return task_id_; }

private:
  int finish_num_ = 0;
  int fail_num_ = 0;
  int task_id_;
  std::string task_name_;
  std::condition_variable cond_;
  std::mutex mutex_;
}; // class Synchronizer

} // namespace
#endif // OPENMI_BASE_SYNCHRONIZER_H_
