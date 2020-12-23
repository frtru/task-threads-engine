#define CATCH_CONFIG_MAIN

#include "engine/TaskThreadsEngine.h"

#include "catch2/catch.hpp"

TEST_CASE("The projects properly link and compile", "[Setup]") {
  REQUIRE(engine::threading::A::foo());
}