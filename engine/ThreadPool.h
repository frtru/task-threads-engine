#pragma once

#include <iostream>
#include <mutex>
#include <queue>
#include <vector>
#include <thread>

namespace engine { namespace threading {

struct Task;

class ThreadPool
{
  // TODO: Maybe add a thread for the scheduler sync with global timers
  // or something else, but it basically would send routines to the
  // worker threads.
  class Scheduler
  {
  public:
    Scheduler(ThreadPool* poolRef)
      : m_poolRef(poolRef) {}

    void AssignTaskToThread(Task* task);

  private:
    ThreadPool* m_poolRef;
  };

  struct WorkerThread
  {
    WorkerThread() = default;
    ~WorkerThread();

    WorkerThread(const WorkerThread& other) = delete;
    WorkerThread& operator=(const WorkerThread& other) = delete;

    void Start();

    std::unique_ptr<std::thread> m_thread;
    std::condition_variable      m_condition;
    std::queue<Task*>            m_tasks;
    std::mutex                   m_queueMutex;
    static bool                  s_stop;
  };

public:
  ThreadPool(unsigned int poolSize = std::thread::hardware_concurrency());

  void EnqueueTask(Task* task) {
    m_scheduler->AssignTaskToThread(task);
  }

private:
  std::vector<WorkerThread>   m_workers;
  std::unique_ptr<Scheduler>  m_scheduler;
};
} // namespace threading
} // namespace engine
