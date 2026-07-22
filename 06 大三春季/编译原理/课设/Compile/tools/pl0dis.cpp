#include <filesystem>
#include <iostream>

#include "pl0/Driver.hpp"

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "Usage: pl0dis <input.pcode>\n";
    return 1;
  }
  std::filesystem::path input_path(argv[1]);
  pl0::InstructionSequence code;
  try {
    code = pl0::load_pcode_file(input_path);
  } catch (const std::exception& ex) {
    std::cerr << ex.what() << '\n';
    return 1;
  }
  pl0::serialize_instructions(code, std::cout);
  std::cout << '\n';
  return 0;
}
