#include "ThreadPool.h"

using namespace engine::threading;

ThreadPool::ThreadPool(unsigned int poolSize) {

}

ThreadPool::~ThreadPool() {

}

ThreadPool::ThreadPool(ThreadPool&& other) noexcept {

}

ThreadPool& ThreadPool::operator=(ThreadPool&& other) noexcept {

}

void ThreadPool::Scheduler::AssignTaskToThread(BaseTaskPtr&& task) {
  // We dont really need to lock to get the size of the queue
  // for a given thread, since the moment we iterate to the next
  // it's gonna be invalid anyways and this is only to get a
  // rough idea of the activity on each thread and balance it.
  // Otherwise we would need to lock ALL the mutexes which would
  // create a lot of contention and horrible performance.
  auto it = std::min_element(
    m_poolRef->m_threads.begin(),
    m_poolRef->m_threads.end(),
    [](Thread& t1, Thread& t2) {
      t1.m_tasks.size() < t2.m_tasks.size();
    }
  );

  std::unique_lock<std::mutex> lock(it->m_queueMutex);
  it->m_tasks.emplace(task);
}