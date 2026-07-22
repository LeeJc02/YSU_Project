#include "catch.hpp"

#include "pl0/Lexer.hpp"
#include "pl0/Parser.hpp"

TEST_CASE("Parser handles if-then-else statements") {
  const char* source = "if x then write(1) else write(0).";
  pl0::DiagnosticSink diagnostics;
  pl0::Lexer lexer(source, diagnostics);
  pl0::Parser parser(lexer, diagnostics);

  auto program = parser.parse_program();
  REQUIRE(program != nullptr);
  REQUIRE(!diagnostics.has_errors());
  REQUIRE(program->block.statements.size() == 1);

  const auto& stmt = *program->block.statements.front();
  const auto* if_stmt = std::get_if<pl0::IfStmt>(&stmt.value);
  REQUIRE(if_stmt != nullptr);
  REQUIRE(if_stmt->then_branch.size() == 1);
  REQUIRE(if_stmt->else_branch.size() == 1);
}

TEST_CASE("Parser understands repeat-until loop") {
  const char* source = "repeat write(i); i := i + 1; until i > 10.";
  pl0::DiagnosticSink diagnostics;
  pl0::Lexer lexer(source, diagnostics);
  pl0::Parser parser(lexer, diagnostics);

  auto program = parser.parse_program();
  REQUIRE(program != nullptr);
  REQUIRE(!diagnostics.has_errors());
  REQUIRE(program->block.statements.size() == 1);

  const auto& stmt = *program->block.statements.front();
  const auto* repeat_stmt = std::get_if<pl0::RepeatStmt>(&stmt.value);
  REQUIRE(repeat_stmt != nullptr);
  REQUIRE(repeat_stmt->body.size() == 2);
}

TEST_CASE("Parser handles compound assignments and increments") {
  const char* source =
      "var x; begin x += 3; x -= 2; x *= 5; x /= 4; x %= 3; x++; x--; end.";
  pl0::DiagnosticSink diagnostics;
  pl0::Lexer lexer(source, diagnostics);
  pl0::Parser parser(lexer, diagnostics);

  auto program = parser.parse_program();
  REQUIRE(program != nullptr);
  REQUIRE(!diagnostics.has_errors());
  REQUIRE(program->block.statements.size() == 1);

  const auto& block_stmt = *program->block.statements.front();
  const auto* begin_block = std::get_if<std::vector<pl0::StmtPtr>>(&block_stmt.value);
  REQUIRE(begin_block != nullptr);
  REQUIRE(begin_block->size() == 7);

  const pl0::AssignmentOperator expected_ops[] = {
      pl0::AssignmentOperator::AddAssign, pl0::AssignmentOperator::SubAssign,
      pl0::AssignmentOperator::MulAssign, pl0::AssignmentOperator::DivAssign,
      pl0::AssignmentOperator::ModAssign, pl0::AssignmentOperator::AddAssign,
      pl0::AssignmentOperator::SubAssign};
  const std::int64_t expected_values[] = {3, 2, 5, 4, 3, 1, 1};

  for (std::size_t i = 0; i < begin_block->size(); ++i) {
    const auto* assign = std::get_if<pl0::AssignmentStmt>(&(*begin_block)[i]->value);
    REQUIRE(assign != nullptr);
    REQUIRE(assign->op == expected_ops[i]);
    const auto* literal =
        std::get_if<pl0::NumberLiteral>(&assign->value->value);
    REQUIRE(literal != nullptr);
    REQUIRE(literal->value == expected_values[i]);
  }
}
