// 文件: Options.hpp
// 功能: 定义编译器与运行时配置项
#pragma once

#include <filesystem>
#include <optional>
#include <string>

namespace pl0 {

// 结构: 编译阶段选项
struct CompilerOptions {
  bool dump_tokens = false;
  bool dump_ast = false;
  bool dump_symbols = false;
  bool dump_pcode = false;
  bool enable_bounds_check = false;
};

// 结构: 运行阶段选项
struct RunnerOptions {
  bool trace_vm = false;
  bool enable_bounds_check = false;
};

// 结构: CLI 解析后的参数
struct CliOptions {
  std::string command;
  std::filesystem::path input;
  std::optional<std::filesystem::path> output;
  CompilerOptions compiler;
  RunnerOptions runtime;
};

}  // namespace pl0
