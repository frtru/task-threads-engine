#include "engine/TaskEngine.h"
#include "engine/Tasks.h"

#include <iostream>
#include <string>

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

  TaskPtr realTask = TaskPtr(new Task(TaskPriority::MEDIUM));
  realTask->Bind(&Yahoo::Bar, &y, 1.23f, "string");

  MacroTask<PriorityTaskQueue> task(TaskPriority::HIGH);
  task.Add(std::move(realTask));
  task.Add(TaskPriority::HIGH, baz, "in baz");

  Task *realTask2 =  new Task(TaskPriority::MEDIUM);
  realTask2->Bind(&Yahoo::Bar, &y, 3.1416f, "mofo");

  TaskEngine::GetInstance()->LaunchTask(BaseTaskPtr(&task));
//  TaskEngine::GetInstance()->LaunchTask(BaseTaskPtr(realTask2));
  int i = 0;
  while (i < 1000000) ++i;

  y.Bar(0.1f, "yoyo");

  return 0;
}