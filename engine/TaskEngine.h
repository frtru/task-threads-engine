#pragma once

#include "Singleton.h"
#include "Tasks.h"
#include "ThreadPool.h"

namespace engine { namespace threading {
class TaskEngine : public Singleton<TaskEngine>
{
public:
  TaskEngine(unsigned int numberOfThreads =
    std::thread::hardware_concurrency())
    : m_pool(numberOfThreads) { }

  void LaunchTask(BaseTaskPtr&& task) {
    m_pool.EnqueueTask(std::move(task));
  }

  static bool LinkSuccessful() { return true; }

private:
  ThreadPool m_pool;
};
} // namespace engine
} // namespace os