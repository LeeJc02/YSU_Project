// 文件: Lexer.hpp
// 功能: 声明词法分析器, 将源码转换为 Token 序列
#pragma once

#include <string>
#include <vector>

#include "pl0/Diagnostics.hpp"
#include "pl0/Token.hpp"

namespace pl0 {

// 类: 逐字符扫描源码并生成 Token
class Lexer {
 public:
  // 构造: 收到源码与诊断收集器
  Lexer(std::string source, DiagnosticSink& diagnostics);

  // 函数: 预读指定位置的 Token
  [[nodiscard]] const Token& peek(std::size_t lookahead = 0);
  // 函数: 取得下一个 Token
  Token next();

  // 函数: 重置扫描状态
  void reset();

 private:
  // 工具: 构造 Token 实例
  Token make_token(TokenKind kind, std::string_view lexeme,
                   SourceLoc start, SourceLoc end);

  // 工具: 跳过空白与注释
  void skip_whitespace_and_comments();

  // 工具: 扫描数字/标识符/符号
  Token lex_number(SourceLoc start);
  Token lex_identifier_or_keyword(SourceLoc start);
  Token lex_symbol(SourceLoc start);

  // 工具: 字符串读游标操作
  char current() const;
  char peek_char(std::size_t offset) const;
  char advance();
  bool is_end() const;

  // 工具: 报告未闭合注释
  void report_unterminated_comment(SourceLoc start);

  // 成员: 源码及扫描状态
  std::string source_;
  DiagnosticSink& diagnostics_;
  std::size_t index_ = 0;
  SourceLoc location_{1, 1};
  SourceLoc token_start_{1, 1};
  std::vector<Token> buffer_;
  bool buffer_valid_ = false;
};

}  // namespace pl0
