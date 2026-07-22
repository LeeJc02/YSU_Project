// 文件: Driver.hpp
// 功能: 提供编译与运行的高层封装接口
#pragma once

#include <filesystem>
#include <iosfwd>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "pl0/AST.hpp"
#include "pl0/Codegen.hpp"
#include "pl0/Diagnostics.hpp"
#include "pl0/Options.hpp"
#include "pl0/PCode.hpp"
#include "pl0/Token.hpp"
#include "pl0/VM.hpp"

namespace pl0 {

// 结构: 控制调试输出内容
struct DumpOptions {
  bool tokens = false;
  bool ast = false;
  bool symbols = false;
  bool pcode = false;
};

// 结构: 编译产物集合
struct CompileResult {
  InstructionSequence code;
  std::vector<Symbol> symbols;
  std::vector<Token> tokens;
  std::unique_ptr<Program> program;
  std::string source_name;
};

// 函数: 从文件编译并可选输出调试信息
CompileResult compile_file(const std::filesystem::path& input,
                           const CompilerOptions& options,
                           const DumpOptions& dumps,
                           DiagnosticSink& diagnostics,
                           std::ostream& dump_stream);

// 函数: 从字符串编译源码
CompileResult compile_source_text(std::string_view source_name,
                                  const std::string& source,
                                  const CompilerOptions& options,
                                  DiagnosticSink& diagnostics);

// 函数: 读取 P-Code 文件
InstructionSequence load_pcode_file(const std::filesystem::path& input);

// 函数: 保存 P-Code 文件
void save_pcode_file(const std::filesystem::path& output,
                     const InstructionSequence& instructions);

// 函数: 执行指令序列
VirtualMachine::Result run_instructions(const InstructionSequence& code,
                                        DiagnosticSink& diagnostics,
                                        const RunnerOptions& options);

// 函数: 打印诊断信息
void print_diagnostics(const DiagnosticSink& diagnostics, std::ostream& out);

}  // namespace pl0
