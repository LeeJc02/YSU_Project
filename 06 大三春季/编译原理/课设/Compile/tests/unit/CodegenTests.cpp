#include "catch.hpp"

#include "TestSupport.hpp"

TEST_CASE("Code generator emits write instruction") {
  pl0::DiagnosticSink diagnostics;
  pl0::CompilerOptions options;
  auto instructions = pl0::test::compile_source("begin write(1); end.", options, diagnostics);
  REQUIRE(!diagnostics.has_errors());

  bool found_write = false;
  for (const auto& instr : instructions) {
    if (instr.op == pl0::Op::OPR && instr.argument == static_cast<int>(pl0::Opr::WRITE)) {
      found_write = true;
      break;
    }
  }
  REQUIRE(found_write);
}

TEST_CASE("Array assignment emits bounds check when enabled") {
  pl0::DiagnosticSink diagnostics;
  pl0::CompilerOptions options;
  options.enable_bounds_check = true;
  auto instructions = pl0::test::compile_source("var a[2]; begin a[1] := 3; end.", options, diagnostics);
  REQUIRE(!diagnostics.has_errors());

  bool found_chk = false;
  for (const auto& instr : instructions) {
    if (instr.op == pl0::Op::CHK) {
      found_chk = true;
      REQUIRE(instr.argument == 2);
      break;
    }
  }
  REQUIRE(found_chk);
}

TEST_CASE("Compound assignment generates arithmetic sequence") {
  pl0::DiagnosticSink diagnostics;
  pl0::CompilerOptions options;
  auto instructions = pl0::test::compile_source("var x; begin x += 2; end.", options, diagnostics);
  REQUIRE(!diagnostics.has_errors());

  bool saw_lod = false;
  bool saw_add = false;
  bool saw_sto = false;
  for (const auto& instr : instructions) {
    if (instr.op == pl0::Op::LOD) {
      saw_lod = true;
    } else if (instr.op == pl0::Op::OPR &&
               instr.argument == static_cast<int>(pl0::Opr::ADD)) {
      saw_add = true;
    } else if (instr.op == pl0::Op::STO) {
      saw_sto = true;
    }
  }
  REQUIRE(saw_lod);
  REQUIRE(saw_add);
  REQUIRE(saw_sto);
}

TEST_CASE("Array compound assignment duplicates address before store") {
  pl0::DiagnosticSink diagnostics;
  pl0::CompilerOptions options;
  options.enable_bounds_check = false;
  const char* source = "var a[2], i; begin i := 0; a[i] += 1; end.";
  auto instructions = pl0::test::compile_source(source, options, diagnostics);
  REQUIRE(!diagnostics.has_errors());

  bool saw_dup = false;
  bool saw_ldi = false;
  bool saw_add = false;
  bool saw_sti = false;
  for (const auto& instr : instructions) {
    if (instr.op == pl0::Op::DUP) {
      saw_dup = true;
    } else if (instr.op == pl0::Op::LDI) {
      saw_ldi = true;
    } else if (instr.op == pl0::Op::OPR &&
               instr.argument == static_cast<int>(pl0::Opr::ADD)) {
      saw_add = true;
    } else if (instr.op == pl0::Op::STI) {
      saw_sti = true;
    }
  }
  REQUIRE(saw_dup);
  REQUIRE(saw_ldi);
  REQUIRE(saw_add);
  REQUIRE(saw_sti);
}

TEST_CASE("Compound assignments map to arithmetic operators") {
  pl0::DiagnosticSink diagnostics;
  pl0::CompilerOptions options;
  const char* source =
      "var x; begin x := 10; x -= 2; x *= 3; x /= 4; x %= 5; end.";
  auto instructions = pl0::test::compile_source(source, options, diagnostics);
  REQUIRE(!diagnostics.has_errors());

  bool saw_sub = false;
  bool saw_mul = false;
  bool saw_div = false;
  bool saw_mod = false;

  for (const auto& instr : instructions) {
    if (instr.op == pl0::Op::OPR) {
      switch (static_cast<pl0::Opr>(instr.argument)) {
        case pl0::Opr::SUB:
          saw_sub = true;
          break;
        case pl0::Opr::MUL:
          saw_mul = true;
          break;
        case pl0::Opr::DIV:
          saw_div = true;
          break;
        case pl0::Opr::MOD:
          saw_mod = true;
          break;
        default:
          break;
      }
    }
  }

  REQUIRE(saw_sub);
  REQUIRE(saw_mul);
  REQUIRE(saw_div);
  REQUIRE(saw_mod);
}
