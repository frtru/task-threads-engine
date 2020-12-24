#include "engine/TaskThreadsEngine.h"
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
  std::cout << A::foo() << std::endl;

  {
    MacroTask<PriorityTaskQueue> task(TaskPriority::HIGH); // Weird that it still requires empty braces even though I'm using C++17

    Yahoo y;

    TaskPtr realTask = TaskPtr(new Task(TaskPriority::MEDIUM));
    realTask->Bind(&Yahoo::Bar, &y, 1.23f, "string");
    task.Add(std::move(realTask));
    task.Add(TaskPriority::HIGH, baz, "in baz");

    task.Run();
  }
  return 0;
}