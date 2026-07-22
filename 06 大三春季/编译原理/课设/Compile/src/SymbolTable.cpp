// 文件: SymbolTable.cpp
// 功能: 实现符号表的作用域管理
#include "pl0/SymbolTable.hpp"

#include <algorithm>

namespace pl0 {

// 构造: 初始化全局作用域
SymbolTable::SymbolTable() {
  scopes_.push_back({0, ScopeInfo{0, 0}});
}

// 函数: 进入新作用域
void SymbolTable::enter_scope() {
  ScopeInfo info;
  if (!scopes_.empty()) {
    info.level = scopes_.back().info.level + 1;
  }
  scopes_.push_back({symbols_.size(), info});
}

// 函数: 离开当前作用域并丢弃符号
void SymbolTable::leave_scope() {
  if (scopes_.empty()) {
    return;
  }
  auto frame = scopes_.back();
  if (symbols_.size() > frame.start_index) {
    symbols_.resize(frame.start_index);
  }
  scopes_.pop_back();
  if (scopes_.empty()) {
    scopes_.push_back({0, ScopeInfo{0, 0}});
  }
}

// 函数: 获取当前作用域信息(可写)
SymbolTable::ScopeInfo& SymbolTable::current_scope() {
  return scopes_.back().info;
}

// 函数: 获取当前作用域信息(只读)
const SymbolTable::ScopeInfo& SymbolTable::current_scope() const {
  return scopes_.back().info;
}

// 函数: 添加符号并返回引用
Symbol& SymbolTable::add_symbol(Symbol symbol) {
  symbol.level = current_scope().level;
  symbols_.push_back(std::move(symbol));
  return symbols_.back();
}

// 函数: 自内向外查找符号
const Symbol* SymbolTable::lookup(const std::string& name) const {
  for (auto it = symbols_.rbegin(); it != symbols_.rend(); ++it) {
    if (it->name == name) {
      return &*it;
    }
  }
  return nullptr;
}

// 函数: 仅在当前作用域搜索符号
const Symbol* SymbolTable::lookup_in_current_scope(const std::string& name) const {
  if (scopes_.empty()) {
    return nullptr;
  }
  const auto frame = scopes_.back();
  for (std::size_t index = symbols_.size(); index > frame.start_index; --index) {
    const auto& symbol = symbols_[index - 1];
    if (symbol.name == name) {
      return &symbol;
    }
  }
  return nullptr;
}

}  // namespace pl0
