// 文件: Diagnostics.cpp
// 功能: 实现诊断收集与格式化输出
#include "pl0/Diagnostics.hpp"

#include <iostream>
#include <sstream>

namespace pl0 {

// 函数: 新增一条诊断
void DiagnosticSink::report(Diagnostic diagnostic) {
  diagnostics_.push_back(std::move(diagnostic));
}

// 函数: 是否存在错误级诊断
bool DiagnosticSink::has_errors() const {
  for (const auto& diag : diagnostics_) {
    if (diag.level == DiagnosticLevel::Error) {
      return true;
    }
  }
  return false;
}

// 函数: 获取诊断列表
const std::vector<Diagnostic>& DiagnosticSink::diagnostics() const {
  return diagnostics_;
}

// 函数: 清空诊断列表
void DiagnosticSink::clear() {
  diagnostics_.clear();
}

// 函数: 将诊断级别转换为文本
static std::string_view level_to_string(DiagnosticLevel level) {
  switch (level) {
    case DiagnosticLevel::Error:
      return "error";
    case DiagnosticLevel::Warning:
      return "warning";
    case DiagnosticLevel::Note:
      return "note";
  }
  return "unknown";
}

// 运算符: 诊断格式化输出
std::ostream& operator<<(std::ostream& os, const Diagnostic& diagnostic) {
  os << level_to_string(diagnostic.level) << " ";
  os << static_cast<int>(diagnostic.code) << ": ";
  os << diagnostic.message;
  os << " (";
  os << diagnostic.range.begin.line << ":" << diagnostic.range.begin.column;
  os << "-";
  os << diagnostic.range.end.line << ":" << diagnostic.range.end.column;
  os << ")";
  return os;
}

}  // namespace pl0
