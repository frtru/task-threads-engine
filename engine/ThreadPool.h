#pragma once

#include "Tasks.h"

#include <mutex>
#include <queue>
#include <vector>
#include <thread>

namespace engine { namespace threading {

struct Thread
{
  std::thread m_thread;
  std::queue<BaseTaskPtr> m_tasks;
  std::mutex m_queueMutex;
};

class ThreadPool
{
  class Scheduler
  {
  public:
    Scheduler(ThreadPool* poolRef)
      : m_poolRef(poolRef) {}

    void AssignTaskToThread(BaseTaskPtr&& task);

  private:
    ThreadPool* m_poolRef;
  };

public:
  ThreadPool(unsigned int poolSize = std::thread::hardware_concurrency());
  ~ThreadPool();

  ThreadPool(const ThreadPool& other) = delete;
  ThreadPool(ThreadPool&& other) noexcept;

  ThreadPool& operator=(const ThreadPool& other) = delete;
  ThreadPool& operator=(ThreadPool&& other) noexcept;

  void EnqueueTask(BaseTaskPtr&& task) {
    m_scheduler->AssignTaskToThread(std::move(task));
  }

private:
  std::vector<Thread> m_threads;
  std::unique_ptr<Scheduler> m_scheduler;
};
} // namespace threading
} // namespace engine
