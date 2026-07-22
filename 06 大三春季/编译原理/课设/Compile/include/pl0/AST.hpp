// 文件: AST.hpp
// 功能: 定义 PL/0 编译器使用的抽象语法树节点结构
#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "pl0/Diagnostics.hpp"

namespace pl0 {

// 枚举: 描述所有二元运算符类型
enum class BinaryOp {
  Add,
  Subtract,
  Multiply,
  Divide,
  Modulo,
  Equal,
  NotEqual,
  Less,
  LessEqual,
  Greater,
  GreaterEqual,
  And,
  Or,
};

// 枚举: 描述所有一元运算符类型
enum class UnaryOp {
  Positive,
  Negative,
  Not,
  Odd,
};

struct Expression;
struct Statement;
struct Block;

using ExprPtr = std::unique_ptr<Expression>;
using StmtPtr = std::unique_ptr<Statement>;

// 结构: 整数字面量节点
struct NumberLiteral {
  std::int64_t value = 0;
};

// 结构: 布尔字面量节点
struct BooleanLiteral {
  bool value = false;
};

// 结构: 标识符引用节点
struct IdentifierExpr {
  std::string name;
};

// 结构: 数组访问节点
struct ArrayAccessExpr {
  std::string name;
  ExprPtr index;
};

// 结构: 二元表达式节点
struct BinaryExpr {
  BinaryOp op;
  ExprPtr lhs;
  ExprPtr rhs;
};

// 结构: 一元表达式节点
struct UnaryExpr {
  UnaryOp op;
  ExprPtr operand;
};

// 结构: 函数调用表达式节点
struct CallExpr {
  std::string callee;
  std::vector<ExprPtr> arguments;
};

// 结构: 表达式统一封装
struct Expression {
  SourceRange range;
  std::variant<NumberLiteral, BooleanLiteral, IdentifierExpr, ArrayAccessExpr,
               BinaryExpr, UnaryExpr, CallExpr>
      value;
};

// 枚举: 赋值运算符种类
enum class AssignmentOperator {
  Assign,
  AddAssign,
  SubAssign,
  MulAssign,
  DivAssign,
  ModAssign,
};

// 结构: 赋值语句节点
struct AssignmentStmt {
  AssignmentOperator op = AssignmentOperator::Assign;
  std::string target;
  ExprPtr index;
  ExprPtr value;
};

// 结构: call 语句节点
struct CallStmt {
  std::string callee;
  std::vector<ExprPtr> arguments;
};

struct BlockStmt;

// 结构: if 语句节点
struct IfStmt {
  ExprPtr condition;
  std::vector<StmtPtr> then_branch;
  std::vector<StmtPtr> else_branch;
};

// 结构: while 语句节点
struct WhileStmt {
  ExprPtr condition;
  std::vector<StmtPtr> body;
};

// 结构: repeat 语句节点
struct RepeatStmt {
  std::vector<StmtPtr> body;
  ExprPtr condition;
};

// 结构: read 语句节点
struct ReadStmt {
  std::vector<std::string> targets;
};

// 结构: write/writeln 语句节点
struct WriteStmt {
  std::vector<ExprPtr> values;
  bool newline = false;
};

// 结构: 统一语句表示
struct Statement {
  SourceRange range;
  std::variant<AssignmentStmt, CallStmt, IfStmt, WhileStmt, RepeatStmt, ReadStmt,
               WriteStmt, std::vector<StmtPtr>>
      value;
};

// 结构: 常量声明节点
struct ConstDecl {
  SourceRange range;
  std::string name;
  std::int64_t value = 0;
};

// 枚举: 变量类型
enum class VarType {
  Integer,
  Boolean,
};

// 结构: 变量声明节点
struct VarDecl {
  SourceRange range;
  std::string name;
  VarType type = VarType::Integer;
  std::optional<std::size_t> array_size;
};

// 结构: 过程声明节点
struct ProcedureDecl {
  SourceRange range;
  std::string name;
  std::vector<VarDecl> parameters;
  std::unique_ptr<Block> body;
};

// 结构: 作用域块节点
struct Block {
  std::vector<ConstDecl> consts;
  std::vector<VarDecl> vars;
  std::vector<ProcedureDecl> procedures;
  std::vector<StmtPtr> statements;
};

// 结构: 程序根节点
struct Program {
  Block block;
};

}  // namespace pl0
