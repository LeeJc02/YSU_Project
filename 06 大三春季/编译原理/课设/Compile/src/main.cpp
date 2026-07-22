// 文件: main.cpp
// 功能: 实现 CLI 前端命令分发
#include <filesystem>
#include <iostream>
#include <optional>
#include <span>
#include <string>
#include <vector>

#include "pl0/Driver.hpp"

namespace {

using pl0::CompileResult;
using pl0::CompilerOptions;
using pl0::DiagnosticSink;
using pl0::DumpOptions;
using pl0::InstructionSequence;
using pl0::RunnerOptions;
using pl0::compile_file;
using pl0::load_pcode_file;
using pl0::print_diagnostics;
using pl0::run_instructions;
using pl0::save_pcode_file;

// 函数: 打印命令行用法
void print_usage() {
  std::cout << "Usage:\n"
            << "  pl0 compile <input.pl0> [-o out.pcode] [--dump-tokens --dump-ast --dump-sym --dump-pcode --bounds-check]\n"
            << "  pl0 run <input.pcode> [--trace-vm]\n"
            << "  pl0 disasm <input.pcode>\n"
            << "  pl0 <input.pl0> [--trace-vm --bounds-check] [--dump-tokens --dump-ast --dump-sym --dump-pcode]\n";
}

// 函数: 根据输入推导默认输出文件
std::filesystem::path default_output(const std::filesystem::path& input) {
  auto output = input;
  output.replace_extension(".pcode");
  return output;
}

// 函数: 处理 compile 子命令
int handle_compile_command(std::span<const std::string> args) {
  if (args.empty()) {
    print_usage();
    return 1;
  }

  CompilerOptions compiler_options;
  DumpOptions dumps;
  std::optional<std::filesystem::path> output_path;
  std::filesystem::path input_path;

  for (std::size_t i = 0; i < args.size(); ++i) {
    const auto& arg = args[i];
    if (arg == "-o" && i + 1 < args.size()) {
      output_path = std::filesystem::path(args[++i]);
    } else if (arg == "--dump-tokens") {
      dumps.tokens = true;
    } else if (arg == "--dump-ast") {
      dumps.ast = true;
    } else if (arg == "--dump-sym") {
      dumps.symbols = true;
    } else if (arg == "--dump-pcode") {
      dumps.pcode = true;
    } else if (arg == "--bounds-check") {
      compiler_options.enable_bounds_check = true;
    } else if (!arg.empty() && arg[0] == '-') {
      std::cerr << "Unknown option: " << arg << '\n';
      return 1;
    } else if (input_path.empty()) {
      input_path = std::filesystem::path(arg);
    } else {
      std::cerr << "Unexpected argument: " << arg << '\n';
      return 1;
    }
  }

  if (input_path.empty()) {
    std::cerr << "No input file specified\n";
    return 1;
  }

  if (!output_path) {
    output_path = default_output(input_path);
  }

  DiagnosticSink diagnostics;
  CompileResult result;
  try {
    result = compile_file(input_path, compiler_options, dumps, diagnostics, std::cout);
  } catch (const std::exception& ex) {
    std::cerr << ex.what() << '\n';
    return 1;
  }

  if (diagnostics.has_errors()) {
    print_diagnostics(diagnostics, std::cerr);
    return 1;
  }

  try {
    save_pcode_file(*output_path, result.code);
  } catch (const std::exception& ex) {
    std::cerr << ex.what() << '\n';
    return 1;
  }
  return 0;
}

// 函数: 处理 run 子命令
int handle_run_command(std::span<const std::string> args) {
  if (args.empty()) {
    print_usage();
    return 1;
  }

  RunnerOptions runner_options;
  std::filesystem::path input_path;

  for (std::size_t i = 0; i < args.size(); ++i) {
    const auto& arg = args[i];
    if (arg == "--trace-vm") {
      runner_options.trace_vm = true;
    } else if (!arg.empty() && arg[0] == '-') {
      std::cerr << "Unknown option: " << arg << '\n';
      return 1;
    } else if (input_path.empty()) {
      input_path = std::filesystem::path(arg);
    } else {
      std::cerr << "Unexpected argument: " << arg << '\n';
      return 1;
    }
  }

  if (input_path.empty()) {
    std::cerr << "No input file specified\n";
    return 1;
  }

  InstructionSequence code;
  try {
    code = load_pcode_file(input_path);
  } catch (const std::exception& ex) {
    std::cerr << ex.what() << '\n';
    return 1;
  }

  DiagnosticSink diagnostics;
  auto result = run_instructions(code, diagnostics, runner_options);
  if (diagnostics.has_errors()) {
    print_diagnostics(diagnostics, std::cerr);
    return 1;
  }
  return result.success ? 0 : 1;
}

// 函数: 处理 disasm 子命令
int handle_disasm_command(std::span<const std::string> args) {
  if (args.size() != 1) {
    print_usage();
    return 1;
  }
  auto input_path = std::filesystem::path(args[0]);
  InstructionSequence code;
  try {
    code = load_pcode_file(input_path);
  } catch (const std::exception& ex) {
    std::cerr << ex.what() << '\n';
    return 1;
  }
  serialize_instructions(code, std::cout);
  std::cout << '\n';
  return 0;
}

// 函数: 处理直接输入源文件的便捷模式
int handle_default_pipeline(std::span<const std::string> args) {
  if (args.empty()) {
    print_usage();
    return 1;
  }

  CompilerOptions compiler_options;
  DumpOptions dumps;
  RunnerOptions runner_options;
  std::filesystem::path input_path;

  for (std::size_t i = 0; i < args.size(); ++i) {
    const auto& arg = args[i];
    if (arg == "--dump-tokens") {
      dumps.tokens = true;
    } else if (arg == "--dump-ast") {
      dumps.ast = true;
    } else if (arg == "--dump-sym") {
      dumps.symbols = true;
    } else if (arg == "--dump-pcode") {
      dumps.pcode = true;
    } else if (arg == "--trace-vm") {
      runner_options.trace_vm = true;
    } else if (arg == "--bounds-check") {
      compiler_options.enable_bounds_check = true;
      runner_options.enable_bounds_check = true;
    } else if (!arg.empty() && arg[0] == '-') {
      std::cerr << "Unknown option: " << arg << '\n';
      return 1;
    } else if (input_path.empty()) {
      input_path = std::filesystem::path(arg);
    } else {
      std::cerr << "Unexpected argument: " << arg << '\n';
      return 1;
    }
  }

  if (input_path.empty()) {
    std::cerr << "No input file specified\n";
    return 1;
  }

  DiagnosticSink diagnostics;
  CompileResult result;
  try {
    result = compile_file(input_path, compiler_options, dumps, diagnostics, std::cout);
  } catch (const std::exception& ex) {
    std::cerr << ex.what() << '\n';
    return 1;
  }

  if (diagnostics.has_errors()) {
    print_diagnostics(diagnostics, std::cerr);
    return 1;
  }

  auto run_result = run_instructions(result.code, diagnostics, runner_options);
  if (diagnostics.has_errors()) {
    print_diagnostics(diagnostics, std::cerr);
    return 1;
  }
  return run_result.success ? 0 : 1;
}

}  // namespace

// 函数: 程序入口, 根据参数选择管线
int main(int argc, char** argv) {
  std::vector<std::string> args;
  args.reserve(static_cast<std::size_t>(argc));
  for (int i = 1; i < argc; ++i) {
    args.emplace_back(argv[i]);
  }

  if (args.empty()) {
    print_usage();
    return 1;
  }

  const std::string& command = args[0];
  if (command == "compile") {
    return handle_compile_command(std::span<const std::string>(args).subspan(1));
  }
  if (command == "run") {
    return handle_run_command(std::span<const std::string>(args).subspan(1));
  }
  if (command == "disasm") {
    return handle_disasm_command(std::span<const std::string>(args).subspan(1));
  }
  return handle_default_pipeline(std::span<const std::string>(args));
}
