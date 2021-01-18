#include "engine/TaskEngine.h"
#include "engine/Pools.h"

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

using namespace engine::threading;

void foo(int test) {
  std::cout << test << std::endl;
}

void baz(std::string test) {
  std::cout << test << std::endl;
}

struct Yahoo
{
  void Bar(float f, std::string s) {
    std::cout << f << s << std::endl;
  }
};

int main(int argc, char* argv[])
{
  //Yahoo y;

  //Task* realTask = new Task(TaskPriority::MEDIUM);
  //realTask->Bind(&Yahoo::Bar, &y, 1.23f, "string");

  //Task *realTask2 =  new Task(TaskPriority::HIGH);
  //realTask2->Bind(baz, "mofo");

  //TaskEngine::GetInstance()->RegisterRoutine(realTask);
  //TaskEngine::GetInstance()->RegisterRoutine(realTask2);

  //y.Bar(0.1f, "yoyo");

  //while (true) {
  //  using namespace std::chrono_literals;
  //  TaskEngine::GetInstance()->Update();
  //  std::this_thread::sleep_for(1000ms);
  //}

  //delete realTask;
  //delete realTask2;

  struct A
  {
    explicit A(std::string i) : _i(i) {}
    std::string _i;
  };

  ObjectPool<A, false> pool(1000, "hello");
  
  std::vector<A*> v1, v2;
  while (pool.FreeChunksCount() > 0) {
    v1.push_back(pool.Create("world"));
  }

  for (A* item : v1) {
    std::cout << item->_i << std::endl;
  }

  unsigned long long i = 0;
  while (v1.size() > 0) {
    ++i;
    pool.Release(std::move(v1.back()));
    v1.pop_back();
  }
  std::cout << i << std::endl;

  while (pool.FreeChunksCount() > 0) {
    v2.push_back(pool.Create("yooy"));
  }

  v2.push_back(pool.Create("woops"));

  for (A* item : v2) {
    std::cout << item->_i << std::endl;
  }

  while (v2.size() > 0) {
    ++i;
    pool.Release(std::move(v2.back()));
    v2.pop_back();
  }

  return 0;
}