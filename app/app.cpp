#include "engine/TaskThreadsEngine.h"
#include "engine/Tasks.h"

#include <iostream>

using namespace engine::threading;

int main(int argc, char* argv[])
{
  std::cout << A::foo() << std::endl;

  {
    MacroTask<> task(ITask::Priority::HIGH); // Weird that it still requires empty braces even though I'm using C++17

    TaskPtr realTask = TaskPtr(new Task);
    task.Add(std::move(realTask));

    task.Run();
  }
  return 0;
}