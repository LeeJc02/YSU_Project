#include "catch.hpp"

#include "pl0/Lexer.hpp"

TEST_CASE("Lexer tokenizes keywords, identifiers, and numbers") {
  pl0::DiagnosticSink diagnostics;
  pl0::Lexer lexer("var answer := 42;", diagnostics);

  auto token1 = lexer.next();
  REQUIRE(token1.kind == pl0::TokenKind::Var);

  auto token2 = lexer.next();
  REQUIRE(token2.kind == pl0::TokenKind::Identifier);
  REQUIRE(token2.lexeme == "answer");

  auto token3 = lexer.next();
  REQUIRE(token3.kind == pl0::TokenKind::Assign);

  auto token4 = lexer.next();
  REQUIRE(token4.kind == pl0::TokenKind::Number);
  REQUIRE(token4.number.value() == 42);
}

TEST_CASE("Lexer skips comments and recognizes booleans") {
  const char* source = "// comment\nvar flag := true; /* block */ flag := false;";
  pl0::DiagnosticSink diagnostics;
  pl0::Lexer lexer(source, diagnostics);

  REQUIRE(lexer.next().kind == pl0::TokenKind::Var);
  REQUIRE(lexer.next().lexeme == "flag");
  REQUIRE(lexer.next().kind == pl0::TokenKind::Assign);
  auto true_token = lexer.next();
  REQUIRE(true_token.kind == pl0::TokenKind::Boolean);
  REQUIRE(true_token.boolean.value());

  // skip semicolon
  lexer.next();

  auto ident = lexer.next();
  REQUIRE(ident.lexeme == "flag");
  auto assign = lexer.next();
  REQUIRE(assign.kind == pl0::TokenKind::Assign);
  auto false_token = lexer.next();
  REQUIRE(false_token.kind == pl0::TokenKind::Boolean);
  REQUIRE(!false_token.boolean.value());
}

TEST_CASE("Lexer recognizes compound assignment operators") {
  const char* source = "x += 1; x -= 2; x *= 3; x /= 4; x %= 5; x++; x--;";
  pl0::DiagnosticSink diagnostics;
  pl0::Lexer lexer(source, diagnostics);

  REQUIRE(lexer.next().kind == pl0::TokenKind::Identifier);
  REQUIRE(lexer.next().kind == pl0::TokenKind::PlusEqual);
  REQUIRE(lexer.next().kind == pl0::TokenKind::Number);
  REQUIRE(lexer.next().kind == pl0::TokenKind::Semicolon);

  REQUIRE(lexer.next().kind == pl0::TokenKind::Identifier);
  REQUIRE(lexer.next().kind == pl0::TokenKind::MinusEqual);
  REQUIRE(lexer.next().kind == pl0::TokenKind::Number);
  REQUIRE(lexer.next().kind == pl0::TokenKind::Semicolon);

  REQUIRE(lexer.next().kind == pl0::TokenKind::Identifier);
  REQUIRE(lexer.next().kind == pl0::TokenKind::StarEqual);
  REQUIRE(lexer.next().kind == pl0::TokenKind::Number);
  REQUIRE(lexer.next().kind == pl0::TokenKind::Semicolon);

  REQUIRE(lexer.next().kind == pl0::TokenKind::Identifier);
  REQUIRE(lexer.next().kind == pl0::TokenKind::SlashEqual);
  REQUIRE(lexer.next().kind == pl0::TokenKind::Number);
  REQUIRE(lexer.next().kind == pl0::TokenKind::Semicolon);

  REQUIRE(lexer.next().kind == pl0::TokenKind::Identifier);
  REQUIRE(lexer.next().kind == pl0::TokenKind::PercentEqual);
  REQUIRE(lexer.next().kind == pl0::TokenKind::Number);
  REQUIRE(lexer.next().kind == pl0::TokenKind::Semicolon);

  REQUIRE(lexer.next().kind == pl0::TokenKind::Identifier);
  REQUIRE(lexer.next().kind == pl0::TokenKind::PlusPlus);
  REQUIRE(lexer.next().kind == pl0::TokenKind::Semicolon);

  REQUIRE(lexer.next().kind == pl0::TokenKind::Identifier);
  REQUIRE(lexer.next().kind == pl0::TokenKind::MinusMinus);
  REQUIRE(lexer.next().kind == pl0::TokenKind::Semicolon);
}
