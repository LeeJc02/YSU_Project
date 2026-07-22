// 文件: Symbol.hpp
// 功能: 声明关键字相关映射
#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

namespace pl0 {

enum class TokenKind : std::uint16_t;

// 枚举: 语言关键字列表
enum class Keyword : std::uint16_t {
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

// 函数: 获取关键字映射表
const std::unordered_map<std::string_view, Keyword>& keyword_table();

// 函数: 按词素查询关键字
std::optional<Keyword> lookup_keyword(std::string_view lexeme);

// 函数: 返回关键字对应 Token 种类
std::optional<TokenKind> keyword_token(Keyword keyword);

}  // namespace pl0
