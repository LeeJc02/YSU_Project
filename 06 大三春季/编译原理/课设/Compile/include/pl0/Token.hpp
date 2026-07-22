// 文件: Token.hpp
// 功能: 定义词法分析阶段产生的 Token 结构
#pragma once

#include <cstdint>
#include <optional>
#include <string>

#include "pl0/Diagnostics.hpp"

namespace pl0 {

// 枚举: 表示全部 Token 类型
enum class TokenKind : std::uint16_t {
  EndOfFile,
  Identifier,
  Number,
  Boolean,
  Plus,
  Minus,
  Star,
  Slash,
  Percent,
  PlusPlus,
  MinusMinus,
  PlusEqual,
  MinusEqual,
  StarEqual,
  SlashEqual,
  PercentEqual,
  Assign,
  Equal,
  NotEqual,
  Less,
  LessEqual,
  Greater,
  GreaterEqual,
  LParen,
  RParen,
  LBracket,
  RBracket,
  Comma,
  Semicolon,
  Period,
  Colon,
  Begin,
  Call,
  Const,
  Do,
  Else,
  End,
  If,
  Odd,
  Procedure,
  Then,
  Var,
  While,
  Repeat,
  Until,
  Read,
  Write,
  Writeln,
  True,
  False,
  And,
  Or,
  Not,
};

// 结构: 词法单元信息
struct Token {
  TokenKind kind = TokenKind::EndOfFile;
  std::string lexeme;
  SourceRange range;
  std::optional<std::int64_t> number;
  std::optional<bool> boolean;
};

// 函数: 将 Token 种类转换为字符串
std::string to_string(TokenKind kind);

}  // namespace pl0
