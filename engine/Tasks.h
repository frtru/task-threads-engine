#pragma once

#include <memory>
#include <queue>

namespace engine { namespace threading {

struct ITask {
  enum Priority : unsigned char {
    IDLE,
    LOW,
    MEDIUM,
    HIGH,
    CRITICAL
  };

  ITask(Priority prio = Priority::MEDIUM)
    : m_prio(prio) {}
  virtual ~ITask() {}

  virtual void Run() = 0;

  Priority m_prio;
};

using TaskPtr = std::unique_ptr<ITask>;
struct PriorityComparison
{
  bool operator()(const TaskPtr& lhs, const TaskPtr& rhs) {
    return lhs->m_prio > rhs->m_prio;
  }
};
using PriorityTaskQueue = std::priority_queue<
  TaskPtr,
  std::deque<TaskPtr>,
  PriorityComparison
>;
using TaskQueue = std::queue<
  TaskPtr
>;

struct Task : ITask {
  Task(Priority prio = Priority::MEDIUM)
    : ITask(prio) {}
  virtual ~Task() override {}

  virtual void Run() override {}
};

template <typename QueueType = TaskQueue>
struct MacroTask : ITask {

  MacroTask(Priority prio = Priority::MEDIUM)
    : ITask(prio) {}

  virtual ~MacroTask() {}
  // Simple enough, we got through the order they were added.
  // The priority_queue sorts them when adding them already
  // and with the normal queue we just have to go through the
  // order they were added
  virtual void Run() override {
    while (!m_queue.empty()) {
      if constexpr (std::is_same<QueueType, PriorityTaskQueue>::value) {
        m_queue.top()->Run();
      }
      else /* Normal queue */ {
        m_queue.front()->Run();
      }
      m_queue.pop();
    }
  }
  
  void Add(TaskPtr task) { 
    m_queue.emplace(std::move(task));
  }

  QueueType m_queue;
};
}
}