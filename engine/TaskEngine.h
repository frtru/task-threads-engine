#pragma once

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

struct Task {
  using FunctionWrapper = std::function<void()>;
  using Ptr = Task*;

  Task(TaskPriority a_prio = TaskPriority::MEDIUM)
    : prio(a_prio) {}

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
    auto task = std::make_shared< FunctionWrapper >(
      std::bind(
        std::forward<F>(func), std::forward<Args>(args)...
      )
      );
    proc = [task]() { (*task)(); };
  }

  inline void Run() { proc(); }

  FunctionWrapper proc;
  TaskPriority    prio;
};

class TaskEngine : public Singleton<TaskEngine>
{
public:
  TaskEngine(
    unsigned int numberOfThreads = std::thread::hardware_concurrency(),
    std::size_t  TaskPoolSize    = 500u);

  void LaunchTask(Task* task);

  static bool LinkSuccessful() { return true; }

private:
//  std::unique_ptr<Task> m_TaskPool; <<---- LOST LOT OF TIME CAUSE THIS WAS CRASHING POST-MAIN
//                                           THIS IS SUPPOSED TO BE A TASK POOL FROM WHICH WE CREATE TASKS
//                                           INSTEAD OF NEW'ING ALL THE TIME
  std::unique_ptr<ThreadPool> m_ThreadPool;
};
} // namespace engine
} // namespace os