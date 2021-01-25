#pragma once

#include "Pools.h"
#include "Singleton.h"
#include "ThreadPool.h"

#include <functional>
#include <iostream>
#include <memory>
#include <thread>

namespace engine { namespace threading {

enum TaskPriority : unsigned char {
  IDLE,
  LOW,
  MEDIUM,
  HIGH,
  CRITICAL
};

#ifndef MAX_SIMULTANEOUS_RUNTIME_TASK_COUNT
#define MAX_SIMULTANEOUS_RUNTIME_TASK_COUNT 500
#endif

struct Task : public PoolableObject<MAX_SIMULTANEOUS_RUNTIME_TASK_COUNT, Task> {
  using FunctionWrapper = std::function<void()>;
  using Ptr = Task*;

  Task(TaskPriority a_prio = TaskPriority::MEDIUM)
    : prio(a_prio) {}

  // We wrap the function and arguments passed
  // into a a pointer for std::function<void()>.
  // This pointer is copied as a capture in the
  // member variable that will then execute the
  // actual function.
  //
  // Were we interested of getting the actual
  // return of the functions, we could use a 
  // std::packaged_task instead of the std::function
  // and return a future of the result of that func.
  // This voluntarily doesn't manage this case as
  // it eases the design.
  template <typename F, typename... Args>
  void Bind(F&& func, Args&&... args) {
    auto task = std::make_shared< FunctionWrapper >(
      std::bind(
        std::forward<F>(func), std::forward<Args>(args)...
      )
      );
    proc = [task]() { (*task)(); };
  }

  inline void Run() { proc(); }

  // TODO: Put children tasks here as a list
  // and queue just after proc() is called in Run

  FunctionWrapper proc;

  // TODO: Decide whether or not I'll use priorities
  TaskPriority    prio;
};

class TaskEngine : public Singleton<TaskEngine>
{
public:
  TaskEngine(unsigned int numberOfThreads = std::thread::hardware_concurrency());

  void LaunchTask(Task* task);
  void RegisterRoutine(Task* task);
  void Update();

  static bool LinkSuccessful() { return true; }

private:
  // NOTE: not the job of the engine to own the task pool
  // as releasing them would be super complicated to manage
  // for little to no reason.
  std::unique_ptr<ThreadPool> m_ThreadPool;
  std::vector<Task*>          m_Routines;
};
} // namespace engine
} // namespace os