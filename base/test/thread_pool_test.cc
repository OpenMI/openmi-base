#include <time.h>
#include "thread_pool.h"
#include "timer.h"
#include <unordered_map>
#include <unistd.h>
#include <signal.h>

using namespace std;

void process_exit(int signal_num) {
  printf("process_exit beging...\n");
  openmi::ThreadPool::Instance().Stop();
  printf("process_exit done.\n");
}

void process_init() {
  printf("process_init beging...\n");
  // signal init
  signal(SIGPIPE, SIG_IGN);
  signal(SIGTERM, process_exit);
  signal(SIGUSR1, process_exit);
  signal(SIGKILL, process_exit);
  //signal(SIGINT, process_exit);  // interrupt. ctrl-c can not stop program
}

double GetTime(void) {
  double t = static_cast<double>(time(NULL)); 
  sleep(1);
  printf("t: %lf\n", t);
  return t;
}

static long NN = 10;

long for_sum(long N) {
  //printf("current thread id: %d, N:%ld\n", std::this_thread::get_id(), N);
  long sum = 0l;
  for (long i = 0; i < N; ++i) {
    sum += i;
    for (int j = 0; j < N; ++j) {
      sum += j*j;
      sum -= (j*j - j/2);
    }
  }
  /*
  if (N / NN == 3) {
    printf("N / NN == 3. call runtime_error\n");
    std::runtime_error("------------ test process_exit ...");
    printf("N / NN == 3. call runtime_error done\n");
  }
  */

  return sum;
}

void ThreadPoolPerfTest() {
  int M = 20;
  long N = NN;
  long sum = 0l;
  openmi::Timer time;
  for (int i = 0; i < M; ++i) {
    sum += for_sum(N*i);
    sum -= for_sum(N);
  }
  printf("single thread: %d us, sum: %ld\n", time.Elapsed(), sum);
  int num_thread = 20;
  openmi::ThreadPool::Instance().Init(num_thread);
  printf("create thread [%d] time:%d\n", num_thread, time.Elapsed());
  std::vector<std::future<long> > vec;
  for (int i = 0; i < M; ++i) {
    vec.emplace_back(openmi::ThreadPool::Instance().AddTask([i, N] { 
      long result = for_sum(N*i); 
      result -= for_sum(N);
      return result; 
    }
    ));
  }
  printf("thread pool. time: %d us, vec[3]:%ld\n", time.Elapsed(), 0l);

  sum = 0l;
  for (int i = 0; i < M; ++i) sum += vec[i].get();
  printf("thread pool. time: %d us, sum: %ld\n", time.Elapsed(), sum);
}

void Insert(std::unordered_map<int, int>& map, long N) {
  for (long i = 0; i < N; ++i) {
    map.insert(std::make_pair((int)i, (int)i));
  }
}

int main(int argc, char** argv) {
  process_init();
  ThreadPoolPerfTest();
  
  /*
  std::unordered_map<int, int> map;
  map.insert(std::make_pair(1,1));
  if (map.find(10) == map.end()) {
    printf("1. 10 not in map\n");
  } else {
    printf("1. 10 in map\n");
  }
  if (map.find(10) == map.end()) {
    printf("2. 10 not in map\n");
  } else {
    printf("2. 10 in map\n");
  }
  */

 /*
  std::unordered_map<int, int> map;
  mit::ThreadPool pool(M/2);
  for (int i = 0; i < M; ++i) {
    pool.AddTask([i, &map, N] { Insert(map, N); });
  }
  printf("\n================================\n");
  for (long i = 0; i < N; ++i) { 
    if (map.find(i) == map.end()) {
      printf("1-key: %ld not in map\n", i);
    } else {
      printf("1-key: %ld, value: %d\n", i, map[i]);
    }
  }
  printf("sleep 1s\n");
  sleep(1);
  printf("\n================================\n");
  for (long i = 0; i < N; ++i) { 
    printf("2-key: %ld, value: %d\n", i, map[i]);
  }
  */
  return 0;
}
