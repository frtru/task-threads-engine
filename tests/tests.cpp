#define CATCH_CONFIG_MAIN

#include "engine/TaskEngine.h"
#include "engine/Pools.h"

#include "catch2/catch.hpp"

TEST_CASE( "The projects properly link and compile", "[Setup]" ) {
  REQUIRE(engine::threading::TaskEngine::LinkSuccessful());
}

TEST_CASE( "Basic ObjectPool(FixedSize) usage", "[Pools]" ) {
  struct A
  {
    A(std::string a_s) : s(a_s) {}
    std::string s;
  };

  ObjectPool<A, false> pool(10, "Hello World!");
  REQUIRE( pool.FreeChunksCount() == 10 );

  SECTION("Creating from max size available.") {
    std::vector<A> v;
    while (pool.FreeChunksCount() > 0) {
      v.push_back(*pool.Create("Hello hell!"));
    }

    REQUIRE( pool.FreeChunksCount() == 0 );
    REQUIRE( v.size() == 10 );
  }

  SECTION("Releasing objects.") {
    std::vector<A> v;
    while (pool.FreeChunksCount() > 0) {
      v.push_back(*pool.Create("Hello hell!"));
    }
    while (v.size() > 0) {
      pool.Release(&v.back());
      v.pop_back();
    }

    REQUIRE( pool.FreeChunksCount() == 10 );
    REQUIRE( v.size() == 0 );
  }

  SECTION("Memory space is reused when releasing and recreating.") {
    A *first = pool.Create("Hello hell!");
    pool.Release(first);
    A *second = pool.Create("Hello hell!");

    REQUIRE( pool.FreeChunksCount() == 9 );
    REQUIRE( first == second );
  }


  SECTION("Creating after fixed size is exceeded returns nullptr.") {
    std::vector<A> v;
    while (pool.FreeChunksCount() > 0) {
      v.push_back(*pool.Create("Hello hell!"));
    }

    A *invalid = pool.Create("Hello");

    REQUIRE( pool.FreeChunksCount() == 0 );
    REQUIRE( invalid == nullptr );
  }
}