// 文件: SymbolTable.hpp
// 功能: 定义符号表及符号元数据
#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "pl0/AST.hpp"

namespace pl0 {

// 枚举: 符号种类
enum class SymbolKind {
  Constant,
  Variable,
  Procedure,
  Parameter,
  Array,
};

// 结构: 符号信息记录
struct Symbol {
  std::string name;
  SymbolKind kind = SymbolKind::Variable;
  VarType type = VarType::Integer;
  int level = 0;
  int address = 0;
  std::size_t size = 1;
  bool by_value = true;
  std::int64_t constant_value = 0;
};

// 类: 层级化符号表管理
class SymbolTable {
 public:
  // 结构: 当前作用域描述
  struct ScopeInfo {
    int level = 0;
    int data_offset = 0;
  };

  SymbolTable();

  // 函数: 进入/离开作用域
  void enter_scope();
  void leave_scope();

  // 函数: 访问当前作用域信息
  [[nodiscard]] ScopeInfo& current_scope();
  [[nodiscard]] const ScopeInfo& current_scope() const;

  // 函数: 添加符号
  Symbol& add_symbol(Symbol symbol);

  // 函数: 查找符号
  [[nodiscard]] const Symbol* lookup(const std::string& name) const;
  [[nodiscard]] const Symbol* lookup_in_current_scope(const std::string& name) const;

  [[nodiscard]] const std::vector<Symbol>& symbols() const { return symbols_; }

 private:
  // 结构: 作用域栈帧信息
  struct ScopeFrame {
    std::size_t start_index = 0;
    ScopeInfo info;
  };

  std::vector<Symbol> symbols_;
  std::vector<ScopeFrame> scopes_;
};

}  // namespace pl0
