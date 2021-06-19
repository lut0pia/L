#include "test.h"

#include "../container/Array.h"
#include "../system/Arguments.h"

using namespace L;

static Array<Test> tests;

void L::add_test(const Test& test) {
  tests.push(test);
}

int L::run_all_tests() {
  uint32_t failures = 0;
  for(const Test& test : tests) {
    log("Running test '%s'", test.name);
    if(!test.func()) {
      warning("Test '%s' failed", test.name);
      failures += 1;
    }
  }

  if(failures > 0) {
    warning("%d/%d tests failed", failures, tests.size());
    return 1;
  } else {
    log("All %d tests ran successfully", tests.size());
    return 0;
  }
}