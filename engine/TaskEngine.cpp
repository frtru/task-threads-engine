#include "TaskEngine.h"

using namespace engine::threading;

TaskEngine::TaskEngine(unsigned int numberOfThreads, std::size_t TaskPoolSize)
  : m_ThreadPool(new ThreadPool(numberOfThreads))
//  , m_TaskPool(new Task[TaskPoolSize])
{
}

void TaskEngine::LaunchTask(Task* task) {
  m_ThreadPool->EnqueueTask(task);
}

void TaskEngine::RegisterRoutine(Task* task) {
  m_Routines.emplace_back(task);
}

void TaskEngine::Update() {
  for (Task *routine : m_Routines) {
    m_ThreadPool->EnqueueTask(routine);
  }
}