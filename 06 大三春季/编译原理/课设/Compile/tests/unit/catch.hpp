#pragma once

#include <exception>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace Catch {

struct TestCase {
  std::string name;
  std::function<void()> func;
};

inline std::vector<TestCase>& registry() {
  static std::vector<TestCase> tests;
  return tests;
}

struct AutoReg {
  AutoReg(const std::string& name, std::function<void()> func) {
    registry().push_back(TestCase{name, std::move(func)});
  }
};

inline int run() {
  int failures = 0;
  for (const auto& test : registry()) {
    try {
      test.func();
      std::cout << "[  OK  ] " << test.name << '\n';
    } catch (const std::exception& ex) {
      ++failures;
      std::cout << "[FAIL ] " << test.name << " - " << ex.what() << '\n';
    }
  }
  std::cout << registry().size() - failures << "/" << registry().size()
            << " tests passed\n";
  return failures;
}

struct TestFailure : std::runtime_error {
  using std::runtime_error::runtime_error;
};

}  // namespace Catch

#define CATCH_CONCAT_IMPL(x, y) x##y
#define CATCH_CONCAT(x, y) CATCH_CONCAT_IMPL(x, y)

#define TEST_CASE(name_literal)                                                   \
  static void CATCH_CONCAT(test_function_, __LINE__)();                          \
  static Catch::AutoReg CATCH_CONCAT(auto_reg_, __LINE__)(                       \
      name_literal, CATCH_CONCAT(test_function_, __LINE__));                     \
  static void CATCH_CONCAT(test_function_, __LINE__)()

#define REQUIRE(condition)                                                        \
  do {                                                                            \
    if (!(condition)) {                                                           \
      std::ostringstream CATCH_CONCAT(_oss_, __LINE__);                           \
      CATCH_CONCAT(_oss_, __LINE__) << "Requirement failed: " #condition;        \
      throw Catch::TestFailure(CATCH_CONCAT(_oss_, __LINE__).str());              \
    }                                                                             \
  } while (false)
