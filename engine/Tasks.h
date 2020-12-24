#pragma once

#include <functional>
#include <future>
#include <memory>
#include <queue>

namespace engine { namespace threading {

enum TaskPriority : unsigned char {
  IDLE,
  LOW,
  MEDIUM,
  HIGH,
  CRITICAL
};

struct ITask {
  ITask(TaskPriority prio = TaskPriority::MEDIUM)
    : m_prio(prio) {}
  virtual ~ITask() {}

  virtual void Run() = 0;

  TaskPriority m_prio;
};

using BaseTaskPtr = std::unique_ptr<ITask>;

struct PriorityComparison
{
  bool operator()(const BaseTaskPtr& lhs, const BaseTaskPtr& rhs) {
    return lhs->m_prio < rhs->m_prio;
  }
};
using PriorityTaskQueue = std::priority_queue<
  BaseTaskPtr,
  std::deque<BaseTaskPtr>,
  PriorityComparison
>;
using TaskQueue = std::queue<
  BaseTaskPtr
>;

struct Task : ITask {
  Task(TaskPriority prio = TaskPriority::MEDIUM)
    : ITask(prio) {}
  virtual ~Task() override {}

  // We wrap the function and arguments passed
  // into a a pointer for std::function<void()>.
  // This is pointer is copied as a capture in 
  // the member variable that will then execute
  // the actual function.
  //
  // Were we interested of getting the actual
  // return of the functions, we could use a 
  // std::packaged_task instead of the std::function
  // and return a future of the result of that func.
  // This voluntarily doesn't manage this case as
  // it eases the design.
  template <typename F, typename... Args>
  void Bind(F&& func, Args&&... args) {
    auto task = std::make_shared< std::function<void()> >(
      std::bind(
        std::forward<F>(func), std::forward<Args>(args)...
      )
    );
    m_proc = [task](){ (*task)(); };
  } 

  virtual void Run() override { m_proc(); }

  std::function<void()> m_proc;
};

using TaskPtr = std::unique_ptr<Task>;

template <typename QueueType = TaskQueue>
struct MacroTask : ITask {

  MacroTask(TaskPriority prio = TaskPriority::MEDIUM)
    : ITask(prio) {}

  virtual ~MacroTask() {}
  // Simple enough, we go through the order they were added.
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
  
  void Add(BaseTaskPtr task) { 
    m_queue.emplace(std::move(task));
  }

  template <typename F, typename... Args>
  void Add(TaskPriority prio, F&& func, Args&&... args) {
    TaskPtr task(new Task(prio));
    task->Bind(func, args...);
    m_queue.emplace(std::move(task));
  }

  QueueType m_queue;
};

using PriorityMacroTaskPtr = std::unique_ptr<MacroTask<PriorityTaskQueue>>;
using MacroTaskPtr = std::unique_ptr<MacroTask<TaskQueue>>;
}
}