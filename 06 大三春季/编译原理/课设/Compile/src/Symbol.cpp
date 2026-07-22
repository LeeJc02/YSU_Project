// 文件: Symbol.cpp
// 功能: 实现关键字映射查询
#include "pl0/Symbol.hpp"

#include <unordered_map>

#include "pl0/Token.hpp"

namespace pl0 {

// 函数: 返回关键字查找表
const std::unordered_map<std::string_view, Keyword>& keyword_table() {
  static const std::unordered_map<std::string_view, Keyword> table{
      {"begin", Keyword::Begin},   {"call", Keyword::Call},
      {"const", Keyword::Const},   {"do", Keyword::Do},
      {"else", Keyword::Else},     {"end", Keyword::End},
      {"if", Keyword::If},         {"odd", Keyword::Odd},
      {"procedure", Keyword::Procedure},
      {"then", Keyword::Then},     {"var", Keyword::Var},
      {"while", Keyword::While},   {"repeat", Keyword::Repeat},
      {"until", Keyword::Until},   {"read", Keyword::Read},
      {"write", Keyword::Write},   {"writeln", Keyword::Writeln},
      {"true", Keyword::True},     {"false", Keyword::False},
      {"and", Keyword::And},       {"or", Keyword::Or},
      {"not", Keyword::Not},
  };
  return table;
}

// 函数: 根据词素匹配关键字
std::optional<Keyword> lookup_keyword(std::string_view lexeme) {
  const auto& table = keyword_table();
  auto it = table.find(lexeme);
  if (it == table.end()) {
    return std::nullopt;
  }
  return it->second;
}

// 函数: 将关键字映射为 Token 种类
std::optional<TokenKind> keyword_token(Keyword keyword) {
  switch (keyword) {
    case Keyword::Begin:
      return TokenKind::Begin;
    case Keyword::Call:
      return TokenKind::Call;
    case Keyword::Const:
      return TokenKind::Const;
    case Keyword::Do:
      return TokenKind::Do;
    case Keyword::Else:
      return TokenKind::Else;
    case Keyword::End:
      return TokenKind::End;
    case Keyword::If:
      return TokenKind::If;
    case Keyword::Odd:
      return TokenKind::Odd;
    case Keyword::Procedure:
      return TokenKind::Procedure;
    case Keyword::Then:
      return TokenKind::Then;
    case Keyword::Var:
      return TokenKind::Var;
    case Keyword::While:
      return TokenKind::While;
    case Keyword::Repeat:
      return TokenKind::Repeat;
    case Keyword::Until:
      return TokenKind::Until;
    case Keyword::Read:
      return TokenKind::Read;
    case Keyword::Write:
      return TokenKind::Write;
    case Keyword::Writeln:
      return TokenKind::Writeln;
    case Keyword::True:
      return TokenKind::True;
    case Keyword::False:
      return TokenKind::False;
    case Keyword::And:
      return TokenKind::And;
    case Keyword::Or:
      return TokenKind::Or;
    case Keyword::Not:
      return TokenKind::Not;
  }
  return std::nullopt;
}

}  // namespace pl0
