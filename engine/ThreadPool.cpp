#include "ThreadPool.h"

#include "TaskEngine.h"

using namespace engine::threading;

bool ThreadPool::WorkerThread::s_stop = false;

void ThreadPool::WorkerThread::Start() {
  m_thread = std::make_unique<std::thread>([this]() {
    for (;;) {
      Task* task;
      {
        std::unique_lock<std::mutex> lock(this->m_queueMutex);

        this->m_condition.wait(lock, [this]() {
          return !m_tasks.empty() || s_stop;
        });

        if (this->s_stop)
          return;

        task = this->m_tasks.front();
        this->m_tasks.pop();
      }

      task->Run();
    }
  });
}

ThreadPool::WorkerThread::~WorkerThread() {
  {
    std::unique_lock<std::mutex> lock(this->m_queueMutex);
    s_stop = true;
  }

  m_condition.notify_all();
  m_thread->join();
}

ThreadPool::ThreadPool(unsigned int poolSize)
  : m_workers(poolSize)
  , m_scheduler(new Scheduler(this)) {
  for (WorkerThread& worker : m_workers) {
    worker.Start();
  }
}

void ThreadPool::Scheduler::AssignTaskToThread(Task* task) {
  // We dont really need to lock to get the size of the queue
  // for a given thread, since the moment we iterate to the next
  // it's gonna be invalid anyways and this is only to get a
  // rough idea of the activity on each thread and balance it.
  // Otherwise we would need to lock ALL the mutexes which would
  // create a lot of contention and horrible performance.
  auto it = std::min_element(
    m_poolRef->m_workers.begin(),
    m_poolRef->m_workers.end(),
    [](WorkerThread& t1, WorkerThread& t2) {
      return t1.m_tasks.size() < t2.m_tasks.size();
    }
  );

  std::unique_lock<std::mutex> lock(it->m_queueMutex);
  it->m_tasks.emplace(task);
  it->m_condition.notify_one();
}