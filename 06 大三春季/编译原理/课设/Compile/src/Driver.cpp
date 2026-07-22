// 文件: Driver.cpp
// 功能: 实现编译流程封装与调试输出工具
#include "pl0/Driver.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <type_traits>

#include "pl0/AST.hpp"
#include "pl0/Lexer.hpp"
#include "pl0/Parser.hpp"
#include "pl0/Token.hpp"
#include "pl0/Utility.hpp"

namespace pl0 {

namespace {

// 函数: 输出指定缩进
void indent(std::ostream& out, int level) {
  for (int i = 0; i < level; ++i) {
    out << "  ";
  }
}

// 函数: 映射二元运算符名称
const char* binary_op_name(BinaryOp op) {
  switch (op) {
    case BinaryOp::Add:
      return "Add";
    case BinaryOp::Subtract:
      return "Subtract";
    case BinaryOp::Multiply:
      return "Multiply";
    case BinaryOp::Divide:
      return "Divide";
    case BinaryOp::Modulo:
      return "Modulo";
    case BinaryOp::Equal:
      return "Equal";
    case BinaryOp::NotEqual:
      return "NotEqual";
    case BinaryOp::Less:
      return "Less";
    case BinaryOp::LessEqual:
      return "LessEqual";
    case BinaryOp::Greater:
      return "Greater";
    case BinaryOp::GreaterEqual:
      return "GreaterEqual";
    case BinaryOp::And:
      return "And";
    case BinaryOp::Or:
      return "Or";
  }
  return "Unknown";
}

// 函数: 映射一元运算符名称
const char* unary_op_name(UnaryOp op) {
  switch (op) {
    case UnaryOp::Positive:
      return "Positive";
    case UnaryOp::Negative:
      return "Negative";
    case UnaryOp::Not:
      return "Not";
    case UnaryOp::Odd:
      return "Odd";
  }
  return "Unknown";
}

// 函数: 映射赋值运算符名称
const char* assignment_op_name(AssignmentOperator op) {
  switch (op) {
    case AssignmentOperator::Assign:
      return "Assign";
    case AssignmentOperator::AddAssign:
      return "AddAssign";
    case AssignmentOperator::SubAssign:
      return "SubAssign";
    case AssignmentOperator::MulAssign:
      return "MulAssign";
    case AssignmentOperator::DivAssign:
      return "DivAssign";
    case AssignmentOperator::ModAssign:
      return "ModAssign";
  }
  return "Assign";
}

// 函数: 打印表达式 AST
void dump_expression(const Expression& expr, std::ostream& out, int level);
// 函数: 打印语句 AST
void dump_statement(const Statement& stmt, std::ostream& out, int level);
// 函数: 打印块 AST
void dump_block(const Block& block, std::ostream& out, int level);

void dump_expression(const Expression& expr, std::ostream& out, int level) {
  std::visit(
      [&](const auto& node) {
        using T = std::decay_t<decltype(node)>;
        if constexpr (std::is_same_v<T, NumberLiteral>) {
          indent(out, level);
          out << "Number " << node.value << '\n';
        } else if constexpr (std::is_same_v<T, BooleanLiteral>) {
          indent(out, level);
          out << "Boolean " << (node.value ? "true" : "false") << '\n';
        } else if constexpr (std::is_same_v<T, IdentifierExpr>) {
          indent(out, level);
          out << "Identifier " << node.name << '\n';
        } else if constexpr (std::is_same_v<T, ArrayAccessExpr>) {
          indent(out, level);
          out << "ArrayAccess " << node.name << '\n';
          dump_expression(*node.index, out, level + 1);
        } else if constexpr (std::is_same_v<T, BinaryExpr>) {
          indent(out, level);
          out << "Binary " << binary_op_name(node.op) << '\n';
          dump_expression(*node.lhs, out, level + 1);
          dump_expression(*node.rhs, out, level + 1);
        } else if constexpr (std::is_same_v<T, UnaryExpr>) {
          indent(out, level);
          out << "Unary " << unary_op_name(node.op) << '\n';
          dump_expression(*node.operand, out, level + 1);
        } else if constexpr (std::is_same_v<T, CallExpr>) {
          indent(out, level);
          out << "CallExpr " << node.callee << '\n';
          for (const auto& arg : node.arguments) {
            dump_expression(*arg, out, level + 1);
          }
        }
      },
      expr.value);
}

void dump_statement(const Statement& stmt, std::ostream& out, int level) {
  std::visit(
      [&](const auto& node) {
        using T = std::decay_t<decltype(node)>;
        if constexpr (std::is_same_v<T, AssignmentStmt>) {
          indent(out, level);
          out << "Assignment " << node.target;
          out << " [" << assignment_op_name(node.op) << "]\n";
          if (node.index) {
            indent(out, level + 1);
            out << "Index" << '\n';
            dump_expression(*node.index, out, level + 2);
          }
          dump_expression(*node.value, out, level + 1);
        } else if constexpr (std::is_same_v<T, CallStmt>) {
          indent(out, level);
          out << "Call " << node.callee << '\n';
          for (const auto& arg : node.arguments) {
            dump_expression(*arg, out, level + 1);
          }
        } else if constexpr (std::is_same_v<T, IfStmt>) {
          indent(out, level);
          out << "If" << '\n';
          dump_expression(*node.condition, out, level + 1);
          indent(out, level);
          out << "Then" << '\n';
          for (const auto& stmt_ptr : node.then_branch) {
            dump_statement(*stmt_ptr, out, level + 1);
          }
          if (!node.else_branch.empty()) {
            indent(out, level);
            out << "Else" << '\n';
            for (const auto& stmt_ptr : node.else_branch) {
              dump_statement(*stmt_ptr, out, level + 1);
            }
          }
        } else if constexpr (std::is_same_v<T, WhileStmt>) {
          indent(out, level);
          out << "While" << '\n';
          dump_expression(*node.condition, out, level + 1);
          for (const auto& stmt_ptr : node.body) {
            dump_statement(*stmt_ptr, out, level + 1);
          }
        } else if constexpr (std::is_same_v<T, RepeatStmt>) {
          indent(out, level);
          out << "Repeat" << '\n';
          for (const auto& stmt_ptr : node.body) {
            dump_statement(*stmt_ptr, out, level + 1);
          }
          indent(out, level);
          out << "Until" << '\n';
          dump_expression(*node.condition, out, level + 1);
        } else if constexpr (std::is_same_v<T, ReadStmt>) {
          indent(out, level);
          out << "Read";
          for (const auto& target : node.targets) {
            out << " " << target;
          }
          out << '\n';
        } else if constexpr (std::is_same_v<T, WriteStmt>) {
          indent(out, level);
          out << (node.newline ? "Writeln" : "Write") << '\n';
          for (const auto& value : node.values) {
            dump_expression(*value, out, level + 1);
          }
        } else if constexpr (std::is_same_v<T, std::vector<StmtPtr>>) {
          indent(out, level);
          out << "Begin" << '\n';
          for (const auto& sub : node) {
            dump_statement(*sub, out, level + 1);
          }
        }
      },
      stmt.value);
}

void dump_block(const Block& block, std::ostream& out, int level) {
  indent(out, level);
  out << "Block" << '\n';
  if (!block.consts.empty()) {
    indent(out, level + 1);
    out << "Consts" << '\n';
    for (const auto& decl : block.consts) {
      indent(out, level + 2);
      out << decl.name << " = " << decl.value << '\n';
    }
  }
  if (!block.vars.empty()) {
    indent(out, level + 1);
    out << "Vars" << '\n';
    for (const auto& decl : block.vars) {
      indent(out, level + 2);
      out << decl.name;
      if (decl.array_size) {
        out << "[" << *decl.array_size << "]";
      }
      out << '\n';
    }
  }
  for (const auto& proc : block.procedures) {
    indent(out, level + 1);
    out << "Procedure " << proc.name << '\n';
    if (proc.body) {
      dump_block(*proc.body, out, level + 2);
    }
  }
  for (const auto& stmt : block.statements) {
    dump_statement(*stmt, out, level + 1);
  }
}

// 函数: 打印词法分析结果
void dump_tokens(const std::vector<Token>& tokens, std::ostream& out) {
  for (const auto& token : tokens) {
    out << token.range.begin.line << ':' << token.range.begin.column << ' ';
    out << to_string(token.kind);
    if (!token.lexeme.empty()) {
      out << " \"" << token.lexeme << "\"";
    }
    if (token.number) {
      out << " = " << *token.number;
    }
    if (token.boolean) {
      out << " = " << (*token.boolean ? "true" : "false");
    }
    out << '\n';
  }
}

// 函数: 打印符号表内容
void dump_symbols(const std::vector<Symbol>& symbols, std::ostream& out) {
  for (const auto& symbol : symbols) {
    out << "level " << symbol.level << " ";
    switch (symbol.kind) {
      case SymbolKind::Constant:
        out << "const " << symbol.name << " = " << symbol.constant_value;
        break;
      case SymbolKind::Variable:
        out << "var " << symbol.name << " @" << symbol.address;
        break;
      case SymbolKind::Array:
        out << "array " << symbol.name << "[" << symbol.size << "] @"
            << symbol.address;
        break;
      case SymbolKind::Procedure:
        out << "proc " << symbol.name << " -> " << symbol.address;
        break;
      case SymbolKind::Parameter:
        out << "param " << symbol.name << " @" << symbol.address;
        break;
    }
    out << '\n';
  }
}

// 函数: 重新扫描源码并收集 Token
std::vector<Token> collect_tokens(const std::string& source) {
  DiagnosticSink sink;
  Lexer lexer(source, sink);
  std::vector<Token> tokens;
  while (true) {
    Token token = lexer.next();
    tokens.push_back(token);
    if (token.kind == TokenKind::EndOfFile) {
      break;
    }
  }
  return tokens;
}

}  // namespace

}  // namespace pl0

// 函数: 编译内存中的源码文本
pl0::CompileResult pl0::compile_source_text(std::string_view source_name,
                                            const std::string& source,
                                            const pl0::CompilerOptions& options,
                                            pl0::DiagnosticSink& diagnostics) {
  pl0::CompileResult result;
  result.source_name = std::string(source_name);

  pl0::Lexer lexer(source, diagnostics);
  pl0::Parser parser(lexer, diagnostics);
  auto program = parser.parse_program();
  if (!program || diagnostics.has_errors()) {
    result.tokens = collect_tokens(source);
    return result;
  }

  pl0::SymbolTable symbols;
  pl0::InstructionSequence instructions;
  pl0::CodeGenerator generator(symbols, instructions, diagnostics, options);
  generator.emit_program(*program);

  result.tokens = collect_tokens(source);

  if (diagnostics.has_errors()) {
    return result;
  }

  result.code = std::move(instructions);
  result.symbols = generator.symbols();
  result.program = std::move(program);
  return result;
}

// 函数: 从文件加载并编译源码
pl0::CompileResult pl0::compile_file(const std::filesystem::path& input,
                                     const pl0::CompilerOptions& options,
                                     const pl0::DumpOptions& dumps,
                                     pl0::DiagnosticSink& diagnostics,
                                     std::ostream& dump_stream) {
  std::string source = pl0::read_file_utf8(input);
  pl0::CompileResult result =
      pl0::compile_source_text(input.string(), source, options, diagnostics);

  if (dumps.tokens && !result.tokens.empty()) {
    pl0::dump_tokens(result.tokens, dump_stream);
  }
  if (dumps.ast && result.program) {
    pl0::dump_block(result.program->block, dump_stream, 0);
  }
  if (dumps.symbols && !result.symbols.empty()) {
    pl0::dump_symbols(result.symbols, dump_stream);
  }
  if (dumps.pcode && !result.code.empty()) {
    pl0::serialize_instructions(result.code, dump_stream);
    dump_stream << '\n';
  }

  return result;
}

// 函数: 从文件读取 P-Code 序列
pl0::InstructionSequence pl0::load_pcode_file(
    const std::filesystem::path& input) {
  std::ifstream file(input);
  if (!file) {
    throw std::runtime_error("failed to open " + input.string());
  }
  return pl0::deserialize_instructions(file);
}

// 函数: 将指令序列保存为文件
void pl0::save_pcode_file(const std::filesystem::path& output,
                          const pl0::InstructionSequence& instructions) {
  std::ofstream file(output);
  if (!file) {
    throw std::runtime_error("failed to open " + output.string());
  }
  pl0::serialize_instructions(instructions, file);
}

// 函数: 执行编译结果
pl0::VirtualMachine::Result pl0::run_instructions(
    const pl0::InstructionSequence& code, pl0::DiagnosticSink& diagnostics,
    const pl0::RunnerOptions& options) {
  pl0::VirtualMachine vm(diagnostics, options);
  return vm.execute(code);
}

// 函数: 输出全部诊断
void pl0::print_diagnostics(const pl0::DiagnosticSink& diagnostics,
                            std::ostream& out) {
  for (const auto& diag : diagnostics.diagnostics()) {
    out << diag << '\n';
  }
}
