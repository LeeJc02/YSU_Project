#include <filesystem>
#include <iostream>
#include <vector>

#include "pl0/Driver.hpp"

int main(int argc, char** argv) {
  std::vector<std::string> args;
  for (int i = 1; i < argc; ++i) {
    args.emplace_back(argv[i]);
  }

  if (args.empty()) {
    std::cerr << "Usage: pl0run <input.pcode> [--trace-vm]\n";
    return 1;
  }

  pl0::RunnerOptions runner_options;
  std::filesystem::path input_path;

  for (const auto& arg : args) {
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

  pl0::InstructionSequence code;
  try {
    code = pl0::load_pcode_file(input_path);
  } catch (const std::exception& ex) {
    std::cerr << ex.what() << '\n';
    return 1;
  }

  pl0::DiagnosticSink diagnostics;
  auto result = pl0::run_instructions(code, diagnostics, runner_options);
  if (diagnostics.has_errors()) {
    pl0::print_diagnostics(diagnostics, std::cerr);
    return 1;
  }
  return result.success ? 0 : 1;
}
