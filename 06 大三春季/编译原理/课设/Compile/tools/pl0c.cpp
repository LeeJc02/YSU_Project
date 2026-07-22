#include <filesystem>
#include <iostream>
#include <optional>
#include <system_error>
#include <vector>

#include "pl0/Driver.hpp"

int main(int argc, char** argv) {
  using pl0::CompileResult;
  using pl0::CompilerOptions;
  using pl0::DiagnosticSink;
  using pl0::DumpOptions;

  std::vector<std::string> args;
  for (int i = 1; i < argc; ++i) {
    args.emplace_back(argv[i]);
  }

  if (args.empty()) {
    std::cerr << "Usage: pl0c <input.pl0> [-o out.pcode] [--dump-tokens --dump-ast --dump-sym --dump-pcode --bounds-check]\n";
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
    auto default_output = input_path;
    default_output.replace_extension(".pcode");
    output_path = default_output;
  }

  DiagnosticSink diagnostics;
  CompileResult result;
  try {
    result = pl0::compile_file(input_path, compiler_options, dumps, diagnostics, std::cout);
  } catch (const std::exception& ex) {
    std::cerr << ex.what() << '\n';
    return 1;
  }

  if (diagnostics.has_errors()) {
    pl0::print_diagnostics(diagnostics, std::cerr);
    return 1;
  }

  try {
    if (output_path->has_parent_path() && !output_path->parent_path().empty()) {
      std::error_code ec;
      std::filesystem::create_directories(output_path->parent_path(), ec);
      if (ec) {
        std::cerr << "failed to create output directory '" << output_path->parent_path().string()
                  << "': " << ec.message() << '\n';
        return 1;
      }
    }
    pl0::save_pcode_file(*output_path, result.code);
  } catch (const std::exception& ex) {
    std::cerr << ex.what() << '\n';
    return 1;
  }

  return 0;
}
