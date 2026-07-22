// 文件: Codegen.cpp
// 功能: 实现 AST 到 P-Code 的指令生成
#include "pl0/Codegen.hpp"

#include <optional>
#include <utility>
#include <variant>

namespace pl0 {

namespace {

// 结构: 多重访问器用于 visit
template <typename... Ts>
struct Overloaded : Ts... {
  using Ts::operator()...;
};

template <typename... Ts>
Overloaded(Ts...) -> Overloaded<Ts...>;

// 函数: 获取复合赋值对应的操作码
std::optional<Opr> operation_for_assignment(AssignmentOperator op) {
  switch (op) {
    case AssignmentOperator::Assign:
      return std::nullopt;
    case AssignmentOperator::AddAssign:
      return Opr::ADD;
    case AssignmentOperator::SubAssign:
      return Opr::SUB;
    case AssignmentOperator::MulAssign:
      return Opr::MUL;
    case AssignmentOperator::DivAssign:
      return Opr::DIV;
    case AssignmentOperator::ModAssign:
      return Opr::MOD;
  }
  return std::nullopt;
}

}  // namespace

// 构造: 绑定符号表、输出缓冲与诊断器
CodeGenerator::CodeGenerator(SymbolTable& symbols, InstructionSequence& output,
                             DiagnosticSink& diagnostics,
                             const CompilerOptions& options)
    : symbols_(symbols), output_(output), diagnostics_(diagnostics), options_(options) {}

// 函数: 生成整個程序
void CodeGenerator::emit_program(const Program& program) {
  emit_block(program.block);
}

// 函数: 写入单条指令并返回索引
int CodeGenerator::emit_instruction(const Instruction& instr) {
  output_.push_back(instr);
  return static_cast<int>(output_.size()) - 1;
}

// 函数: 回填跳转目标
void CodeGenerator::patch(int index, int target) {
  if (index >= 0 && index < static_cast<int>(output_.size())) {
    output_[static_cast<std::size_t>(index)].argument = target;
  }
}

// 函数: 生成块级代码, 包含声明与语句
void CodeGenerator::emit_block(const Block& block) {
  symbols_.enter_scope();
  auto& scope = symbols_.current_scope();
  scope.data_offset = 3;

  int jump_index = emit_instruction({Op::JMP, 0, 0});

  for (const auto& decl : block.consts) {
    emit_const(decl);
  }

  for (const auto& decl : block.vars) {
    emit_var(decl);
  }

  struct ProcedureContext {
    const ProcedureDecl* decl;
    Symbol* symbol;
  };

  std::vector<ProcedureContext> procedures;
  procedures.reserve(block.procedures.size());

  for (const auto& proc : block.procedures) {
    if (symbols_.lookup_in_current_scope(proc.name)) {
      diagnostics_.report({DiagnosticLevel::Error, DiagnosticCode::Redeclaration,
                           "redeclaration of procedure '" + proc.name + "'",
                           proc.range});
      continue;
    }
    Symbol symbol;
    symbol.name = proc.name;
    symbol.kind = SymbolKind::Procedure;
    symbol.address = 0;
    symbol.size = 0;
    auto& entry = symbols_.add_symbol(std::move(symbol));
    procedures.push_back({&proc, &entry});
  }

  for (auto& proc : procedures) {
    emit_procedure(*proc.decl, *proc.symbol);
  }

  patch(jump_index, static_cast<int>(output_.size()));

  emit_instruction({Op::INT, 0, scope.data_offset});
  emit_statements(block.statements);
  emit_instruction({Op::OPR, 0, static_cast<int>(Opr::RET)});

  symbols_.leave_scope();
}

// 函数: 调度语句生成
void CodeGenerator::emit_statement(const Statement& stmt) {
  std::visit(
      Overloaded{
          [&](const AssignmentStmt& assignment) {
            emit_assignment(assignment, stmt.range);
          },
          [&](const CallStmt& call) { emit_call(call.callee, call.arguments, stmt.range); },
          [&](const IfStmt& conditional) { emit_if(conditional); },
          [&](const WhileStmt& loop) { emit_while(loop); },
          [&](const RepeatStmt& loop) { emit_repeat(loop); },
          [&](const ReadStmt& read) { emit_read(read, stmt.range); },
          [&](const WriteStmt& write) { emit_write(write); },
          [&](const std::vector<StmtPtr>& block) { emit_statements(block); }},
      stmt.value);
}

// 函数: 依次生成语句列表
void CodeGenerator::emit_statements(const std::vector<StmtPtr>& stmts) {
  for (const auto& stmt : stmts) {
    if (stmt) {
      emit_statement(*stmt);
    }
  }
}

// 函数: 生成赋值语句, 支持数组/复合操作
void CodeGenerator::emit_assignment(const AssignmentStmt& stmt,
                                    const SourceRange& range) {
  const Symbol* symbol = resolve(stmt.target, range);
  if (!symbol) {
    return;
  }
  if (symbol->kind == SymbolKind::Constant) {
    diagnostics_.report({DiagnosticLevel::Error, DiagnosticCode::InvalidAssignmentTarget,
                         "cannot assign to constant '" + stmt.target + "'",
                         range});
    return;
  }

  int level_diff = symbols_.current_scope().level - symbol->level;
  auto compound = operation_for_assignment(stmt.op);
  if (stmt.index) {
    if (symbol->kind != SymbolKind::Array) {
      diagnostics_.report({DiagnosticLevel::Error, DiagnosticCode::InvalidArraySubscript,
                           "identifier '" + stmt.target + "' is not an array",
                           range});
      return;
    }
    emit_instruction({Op::LDA, level_diff, symbol->address});
    emit_expression(*stmt.index);
    if (options_.enable_bounds_check && symbol->size > 0) {
      emit_instruction({Op::CHK, 0, static_cast<int>(symbol->size)});
    }
    emit_instruction({Op::IDX, 0, 0});
    if (!compound) {
      emit_expression(*stmt.value);
    } else {
      emit_instruction({Op::DUP, 0, 0});
      emit_instruction({Op::LDI, 0, 0});
      emit_expression(*stmt.value);
      emit_instruction({Op::OPR, 0, static_cast<int>(*compound)});
    }
    emit_instruction({Op::STI, 0, 0});
  } else {
    if (!compound) {
      emit_expression(*stmt.value);
    } else {
      emit_instruction({Op::LOD, level_diff, symbol->address});
      emit_expression(*stmt.value);
      emit_instruction({Op::OPR, 0, static_cast<int>(*compound)});
    }
    emit_instruction({Op::STO, level_diff, symbol->address});
  }
}

// 函数: 生成过程调用指令
void CodeGenerator::emit_call(const std::string& callee,
                              const std::vector<ExprPtr>& arguments,
                              const SourceRange& range) {
  const Symbol* symbol = resolve(callee, range);
  if (!symbol) {
    return;
  }
  if (symbol->kind != SymbolKind::Procedure) {
    diagnostics_.report({DiagnosticLevel::Error, DiagnosticCode::InvalidAssignmentTarget,
                         "identifier '" + callee + "' is not a procedure",
                         range});
    return;
  }
  if (!arguments.empty()) {
    diagnostics_.report({DiagnosticLevel::Error, DiagnosticCode::UnexpectedToken,
                         "procedure parameters are not supported yet", range});
  }
  int level_diff = symbols_.current_scope().level - symbol->level;
  emit_instruction({Op::CAL, level_diff, symbol->address});
}

// 函数: 生成 if/else 控制流
void CodeGenerator::emit_if(const IfStmt& stmt) {
  emit_expression(*stmt.condition);
  int else_jump = emit_instruction({Op::JPC, 0, 0});
  emit_statements(stmt.then_branch);
  int end_jump = -1;
  if (!stmt.else_branch.empty()) {
    end_jump = emit_instruction({Op::JMP, 0, 0});
    patch(else_jump, static_cast<int>(output_.size()));
    emit_statements(stmt.else_branch);
  } else {
    patch(else_jump, static_cast<int>(output_.size()));
  }
  if (end_jump >= 0) {
    patch(end_jump, static_cast<int>(output_.size()));
  }
}

// 函数: 生成 while 循环控制流
void CodeGenerator::emit_while(const WhileStmt& stmt) {
  int loop_start = static_cast<int>(output_.size());
  emit_expression(*stmt.condition);
  int exit_jump = emit_instruction({Op::JPC, 0, 0});
  emit_statements(stmt.body);
  emit_instruction({Op::JMP, 0, loop_start});
  patch(exit_jump, static_cast<int>(output_.size()));
}

// 函数: 生成 repeat 循环控制流
void CodeGenerator::emit_repeat(const RepeatStmt& stmt) {
  int loop_start = static_cast<int>(output_.size());
  emit_statements(stmt.body);
  emit_expression(*stmt.condition);
  emit_instruction({Op::JPC, 0, loop_start});
}

// 函数: 处理 read 语句并生成输入指令
void CodeGenerator::emit_read(const ReadStmt& stmt, const SourceRange& range) {
  for (const auto& name : stmt.targets) {
    const Symbol* symbol = resolve(name, range);
    if (!symbol) {
      continue;
    }
    if (symbol->kind == SymbolKind::Constant) {
      diagnostics_.report({DiagnosticLevel::Error, DiagnosticCode::InvalidAssignmentTarget,
                           "cannot read into constant '" + name + "'", range});
      continue;
    }
    int level_diff = symbols_.current_scope().level - symbol->level;
    emit_instruction({Op::OPR, 0, static_cast<int>(Opr::READ)});
    emit_instruction({Op::STO, level_diff, symbol->address});
  }
}

// 函数: 处理 write/writeln 输出
void CodeGenerator::emit_write(const WriteStmt& stmt) {
  for (const auto& value : stmt.values) {
    emit_expression(*value);
    emit_instruction({Op::OPR, 0, static_cast<int>(Opr::WRITE)});
  }
  if (stmt.newline) {
    emit_instruction({Op::OPR, 0, static_cast<int>(Opr::WRITELN)});
  }
}

// 函数: 遍历表达式节点生成指令
void CodeGenerator::emit_expression(const Expression& expr) {
  std::visit(
      Overloaded{
          [&](const NumberLiteral& literal) {
            emit_instruction({Op::LIT, 0, static_cast<int>(literal.value)});
          },
          [&](const BooleanLiteral& literal) {
            emit_instruction({Op::LIT, 0, literal.value ? 1 : 0});
          },
          [&](const IdentifierExpr& ident) { emit_identifier(ident, expr.range); },
          [&](const ArrayAccessExpr& access) {
            emit_array_access(access, expr.range, true);
          },
          [&](const BinaryExpr& binary) { emit_binary(binary); },
          [&](const UnaryExpr& unary) { emit_unary(unary); },
          [&](const CallExpr&) {
            diagnostics_.report({DiagnosticLevel::Error,
                                 DiagnosticCode::UnexpectedToken,
                                 "procedure call cannot be used as expression",
                                 expr.range});
          }},
      expr.value);
}

// 函数: 对二元运算生成对应 OPR
void CodeGenerator::emit_binary(const BinaryExpr& expr) {
  emit_expression(*expr.lhs);
  emit_expression(*expr.rhs);
  Opr operation;
  switch (expr.op) {
    case BinaryOp::Add:
      operation = Opr::ADD;
      break;
    case BinaryOp::Subtract:
      operation = Opr::SUB;
      break;
    case BinaryOp::Multiply:
      operation = Opr::MUL;
      break;
    case BinaryOp::Divide:
      operation = Opr::DIV;
      break;
    case BinaryOp::Modulo:
      operation = Opr::MOD;
      break;
    case BinaryOp::Equal:
      operation = Opr::EQ;
      break;
    case BinaryOp::NotEqual:
      operation = Opr::NE;
      break;
    case BinaryOp::Less:
      operation = Opr::LT;
      break;
    case BinaryOp::LessEqual:
      operation = Opr::LE;
      break;
    case BinaryOp::Greater:
      operation = Opr::GT;
      break;
    case BinaryOp::GreaterEqual:
      operation = Opr::GE;
      break;
    case BinaryOp::And:
      operation = Opr::AND;
      break;
    case BinaryOp::Or:
      operation = Opr::OR;
      break;
  }
  emit_instruction({Op::OPR, 0, static_cast<int>(operation)});
}

// 函数: 对一元运算生成对应 OPR
void CodeGenerator::emit_unary(const UnaryExpr& expr) {
  emit_expression(*expr.operand);
  Opr operation;
  switch (expr.op) {
    case UnaryOp::Positive:
      return;
    case UnaryOp::Negative:
      operation = Opr::NEG;
      break;
    case UnaryOp::Not:
      operation = Opr::NOT;
      break;
    case UnaryOp::Odd:
      operation = Opr::ODD;
      break;
  }
  emit_instruction({Op::OPR, 0, static_cast<int>(operation)});
}

// 函数: 按标识符种类加载值
void CodeGenerator::emit_identifier(const IdentifierExpr& expr,
                                    const SourceRange& range) {
  const Symbol* symbol = resolve(expr.name, range);
  if (!symbol) {
    return;
  }
  int level_diff = symbols_.current_scope().level - symbol->level;
  switch (symbol->kind) {
    case SymbolKind::Constant:
      emit_instruction({Op::LIT, 0, static_cast<int>(symbol->constant_value)});
      break;
    case SymbolKind::Variable:
    case SymbolKind::Parameter:
      emit_instruction({Op::LOD, level_diff, symbol->address});
      break;
    case SymbolKind::Array:
      diagnostics_.report({DiagnosticLevel::Error, DiagnosticCode::InvalidArraySubscript,
                           "array '" + expr.name + "' requires an index", range});
      break;
    case SymbolKind::Procedure:
      diagnostics_.report({DiagnosticLevel::Error, DiagnosticCode::InvalidAssignmentTarget,
                           "procedure '" + expr.name + "' cannot be used as value",
                           range});
      break;
  }
}

// 函数: 数组访问生成地址与可选加载
void CodeGenerator::emit_array_access(const ArrayAccessExpr& expr,
                                      const SourceRange& range, bool load_value) {
  const Symbol* symbol = resolve(expr.name, range);
  if (!symbol) {
    return;
  }
  if (symbol->kind != SymbolKind::Array) {
    diagnostics_.report({DiagnosticLevel::Error, DiagnosticCode::InvalidArraySubscript,
                         "identifier '" + expr.name + "' is not an array", range});
    return;
  }
  int level_diff = symbols_.current_scope().level - symbol->level;
  emit_instruction({Op::LDA, level_diff, symbol->address});
  emit_expression(*expr.index);
  if (options_.enable_bounds_check && symbol->size > 0) {
    emit_instruction({Op::CHK, 0, static_cast<int>(symbol->size)});
  }
  emit_instruction({Op::IDX, 0, 0});
  if (load_value) {
    emit_instruction({Op::LDI, 0, 0});
  }
}

// 函数: 记录常量并检测重定义
void CodeGenerator::emit_const(const ConstDecl& decl) {
  if (symbols_.lookup_in_current_scope(decl.name)) {
    diagnostics_.report({DiagnosticLevel::Error, DiagnosticCode::Redeclaration,
                         "redeclaration of '" + decl.name + "'", decl.range});
    return;
  }
  Symbol symbol;
  symbol.name = decl.name;
  symbol.kind = SymbolKind::Constant;
  symbol.constant_value = decl.value;
  symbol.size = 1;
  symbol.type = VarType::Integer;
  auto& stored = symbols_.add_symbol(std::move(symbol));
  exported_symbols_.push_back(stored);
}

// 函数: 记录变量或数组声明
void CodeGenerator::emit_var(const VarDecl& decl) {
  if (symbols_.lookup_in_current_scope(decl.name)) {
    diagnostics_.report({DiagnosticLevel::Error, DiagnosticCode::Redeclaration,
                         "redeclaration of '" + decl.name + "'", decl.range});
    return;
  }
  std::size_t size = decl.array_size.value_or(1);
  if (size == 0) {
    diagnostics_.report({DiagnosticLevel::Error, DiagnosticCode::InvalidArraySubscript,
                         "array size must be positive", decl.range});
    size = 1;
  }
  auto& scope = symbols_.current_scope();
  Symbol symbol;
  symbol.name = decl.name;
  symbol.kind = decl.array_size ? SymbolKind::Array : SymbolKind::Variable;
  symbol.address = scope.data_offset;
  symbol.size = size;
  symbol.type = decl.type;
  auto& stored = symbols_.add_symbol(std::move(symbol));
  exported_symbols_.push_back(stored);
  scope.data_offset += static_cast<int>(size);
}

// 函数: 为过程分配入口并生成函数体
void CodeGenerator::emit_procedure(const ProcedureDecl& decl, Symbol& symbol) {
  symbol.address = static_cast<int>(output_.size());
  exported_symbols_.push_back(symbol);
  if (decl.body) {
    emit_block(*decl.body);
  }
}

// 函数: 查找符号并在缺失时报告错误
const Symbol* CodeGenerator::resolve(const std::string& name,
                                     const SourceRange& range) const {
  const Symbol* symbol = symbols_.lookup(name);
  if (!symbol) {
    diagnostics_.report({DiagnosticLevel::Error, DiagnosticCode::UndeclaredIdentifier,
                         "undeclared identifier '" + name + "'", range});
  }
  return symbol;
}

}  // namespace pl0
