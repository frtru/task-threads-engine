#pragma once

#include "Singleton.h"
#include "Tasks.h"
#include "ThreadPool.h"

namespace engine { namespace threading {
class TaskEngine : public Singleton<TaskEngine>
{
public:
  TaskEngine(unsigned int numberOfThreads = std::thread::hardware_concurrency());
  ~TaskEngine();

  TaskEngine(const TaskEngine& other) = delete;
  TaskEngine(TaskEngine&& other) noexcept;

  TaskEngine& operator=(const TaskEngine& other) = delete;
  TaskEngine& operator=(TaskEngine&& other) noexcept;

private:
  ThreadPool m_pool;

  
};
} // namespace engine
} // namespace os