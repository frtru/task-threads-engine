#include "engine/TaskEngine.h"

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
  Yahoo y;

  Task* realTask = new Task(TaskPriority::MEDIUM);
  realTask->Bind(&Yahoo::Bar, &y, 1.23f, "string");

  Task *realTask2 =  new Task(TaskPriority::HIGH);
  realTask2->Bind(baz, "mofo");

  TaskEngine::GetInstance()->RegisterRoutine(realTask);
  TaskEngine::GetInstance()->RegisterRoutine(realTask2);

  y.Bar(0.1f, "yoyo");

  while (true) {
    using namespace std::chrono_literals;
    TaskEngine::GetInstance()->Update();
    std::this_thread::sleep_for(1000ms);
  }

  delete realTask;
  delete realTask2;

  return 0;
}