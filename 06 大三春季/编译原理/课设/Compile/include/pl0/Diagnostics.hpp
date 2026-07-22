// 文件: Diagnostics.hpp
// 功能: 定义诊断信息结构与收集器
#pragma once

#include <cstdint>
#include <ostream>
#include <string>
#include <string_view>
#include <vector>

namespace pl0 {

// 结构: 表示源码中的单个位置
struct SourceLoc {
  std::size_t line = 1;
  std::size_t column = 1;
};

// 结构: 表示源码区间
struct SourceRange {
  SourceLoc begin;
  SourceLoc end;
};

// 枚举: 诊断级别
enum class DiagnosticLevel {
  Error,
  Warning,
  Note,
};

// 枚举: 诊断代码
enum class DiagnosticCode : std::uint16_t {
  UnexpectedToken,
  UnterminatedComment,
  InvalidNumber,
  Redeclaration,
  UndeclaredIdentifier,
  ExpectedIdentifier,
  ExpectedSymbol,
  InvalidAssignmentTarget,
  InvalidArraySubscript,
  StackOverflow,
  StackUnderflow,
  DivisionByZero,
  RuntimeError,
  IOError,
  InternalError,
};

// 结构: 单条诊断信息
struct Diagnostic {
  DiagnosticLevel level = DiagnosticLevel::Error;
  DiagnosticCode code = DiagnosticCode::InternalError;
  std::string message;
  SourceRange range;
};

// 类: 收集与查询诊断记录
class DiagnosticSink {
 public:
  // 函数: 上报一条诊断
  void report(Diagnostic diagnostic);

  // 函数: 是否存在错误级诊断
  [[nodiscard]] bool has_errors() const;

  // 函数: 获取全部诊断
  [[nodiscard]] const std::vector<Diagnostic>& diagnostics() const;

  // 函数: 清空诊断
  void clear();

 private:
  std::vector<Diagnostic> diagnostics_;
};

// 运算符: 诊断格式化输出
std::ostream& operator<<(std::ostream& os, const Diagnostic& diagnostic);

}  // namespace pl0
