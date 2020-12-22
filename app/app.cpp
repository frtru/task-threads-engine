#include "engine/TaskThreadsEngine.h"

#include <iostream>

int main(int argc, char* argv[])
{
  std::cout << os::threading::A::foo() << std::endl;
  return 0;
}