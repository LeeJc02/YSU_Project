#include "catch.hpp"

#include "TestSupport.hpp"
#include "pl0/Driver.hpp"

#include <iostream>
#include <sstream>

TEST_CASE("Virtual machine executes program and produces expected output") {
  const char* source = "var x; begin x := 1; x := x + 2; write(x); end.";
  pl0::CompilerOptions compiler_options;
  pl0::DiagnosticSink diagnostics;
  auto instructions = pl0::test::compile_source(source, compiler_options, diagnostics);
  REQUIRE(!diagnostics.has_errors());

  pl0::RunnerOptions runner_options;
  std::ostringstream capture;
  auto* previous_buf = std::cout.rdbuf(capture.rdbuf());
  auto result = pl0::run_instructions(instructions, diagnostics, runner_options);
  std::cout.rdbuf(previous_buf);
  REQUIRE(!diagnostics.has_errors());
  REQUIRE(result.success);
  REQUIRE(result.last_value == 3);
  REQUIRE(capture.str() == "3");
}

TEST_CASE("Virtual machine handles compound assignments and increments") {
  const char* source =
      "var x; begin x := 10; x -= 3; x *= 2; x /= 7; x %= 3; x++; x--; write(x); end.";
  pl0::CompilerOptions compiler_options;
  pl0::DiagnosticSink diagnostics;
  auto instructions = pl0::test::compile_source(source, compiler_options, diagnostics);
  REQUIRE(!diagnostics.has_errors());

  pl0::RunnerOptions runner_options;
  std::ostringstream capture;
  auto* previous_buf = std::cout.rdbuf(capture.rdbuf());
  auto result = pl0::run_instructions(instructions, diagnostics, runner_options);
  std::cout.rdbuf(previous_buf);
  REQUIRE(!diagnostics.has_errors());
  REQUIRE(result.success);
  REQUIRE(result.last_value == 2);
  REQUIRE(capture.str() == "2");
}
