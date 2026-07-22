// 文件: Codegen.hpp
// 功能: 声明代码生成器, 将 AST 转换为 P-Code 指令
#pragma once

#include <cstdint>
#include <vector>

#include "pl0/AST.hpp"
#include "pl0/Diagnostics.hpp"
#include "pl0/Options.hpp"
#include "pl0/PCode.hpp"
#include "pl0/SymbolTable.hpp"

namespace pl0 {

// 类: 遍历 AST 并生成对应的 P-Code 指令流
class CodeGenerator {
 public:
  // 构造: 持有符号表、指令容器与诊断句柄
  CodeGenerator(SymbolTable& symbols, InstructionSequence& output,
                DiagnosticSink& diagnostics, const CompilerOptions& options);

  // 函数: 处理完整程序节点
  void emit_program(const Program& program);

  [[nodiscard]] const std::vector<Symbol>& symbols() const { return exported_symbols_; }

 private:
  // 工具: 指令写入与回填
  int emit_instruction(const Instruction& instr);
  void patch(int index, int target);
  // 工具: 各种节点生成例程
  void emit_block(const Block& block);
  void emit_statement(const Statement& stmt);
  void emit_statements(const std::vector<StmtPtr>& stmts);
  void emit_assignment(const AssignmentStmt& stmt, const SourceRange& range);
  void emit_call(const std::string& callee,
                 const std::vector<ExprPtr>& arguments,
                 const SourceRange& range);
  void emit_if(const IfStmt& stmt);
  void emit_while(const WhileStmt& stmt);
  void emit_repeat(const RepeatStmt& stmt);
  void emit_read(const ReadStmt& stmt, const SourceRange& range);
  void emit_write(const WriteStmt& stmt);
  void emit_expression(const Expression& expr);
  void emit_binary(const BinaryExpr& expr);
  void emit_unary(const UnaryExpr& expr);
  void emit_identifier(const IdentifierExpr& expr, const SourceRange& range);
  void emit_array_access(const ArrayAccessExpr& expr, const SourceRange& range,
                         bool load_value);
  void emit_const(const ConstDecl& decl);
  void emit_var(const VarDecl& decl);
  void emit_procedure(const ProcedureDecl& decl, Symbol& symbol);

  // 工具: 名称查找
  const Symbol* resolve(const std::string& name, const SourceRange& range) const;

  // 成员: 共享状态
  SymbolTable& symbols_;
  InstructionSequence& output_;
  DiagnosticSink& diagnostics_;
  const CompilerOptions& options_;
  std::vector<Symbol> exported_symbols_;
};

}  // namespace pl0
