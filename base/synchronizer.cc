#include "synchronizer.h" 

namespace openmi {

void Synchronizer::Wait(int total_num) {
  std::unique_lock<std::mutex> lock(mutex_);
  while (total_num != finish_num_) {
    cond_.wait(lock);
  }
  if (fail_num_) {
    throw std::runtime_error(
      "task_name[" + task_name_ + "]: [" + std::to_string(fail_num_) 
      + "] task(s) failed in all [" + std::to_string(total_num) + "] tasks!"
    );
  }
} // Wait

void Synchronizer::AddFinishOne(bool task_result) {
  std::lock_guard<std::mutex> lock(mutex_); 
  {
    finish_num_++;    // whatever success or failure 
    if (!task_result) fail_num_++;
  }
  cond_.notify_one();
} // AddFinishOne

void Synchronizer::Clear() {
  finish_num_ = 0;
  fail_num_ = 0;
}

} // namespace 
