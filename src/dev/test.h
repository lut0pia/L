#pragma once

namespace L {
  using TestFunc = bool();

  struct Test {
    const char* name;
    TestFunc* func;
  };

  void add_test(const Test& test);

  int run_all_tests();
}
