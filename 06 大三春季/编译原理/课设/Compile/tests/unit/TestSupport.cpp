#include "TestSupport.hpp"

#include <string>

#include "pl0/Lexer.hpp"
#include "pl0/Parser.hpp"

namespace pl0::test {

InstructionSequence compile_source(std::string_view source,
                                   const CompilerOptions& options,
                                   DiagnosticSink& diagnostics) {
  Lexer lexer(std::string(source), diagnostics);
  Parser parser(lexer, diagnostics);
  auto program = parser.parse_program();
  if (!program || diagnostics.has_errors()) {
    return {};
  }

  SymbolTable symbols;
  InstructionSequence instructions;
  CodeGenerator generator(symbols, instructions, diagnostics, options);
  generator.emit_program(*program);
  if (diagnostics.has_errors()) {
    return {};
  }
  return instructions;
}

void initialize_environment() {}

}  // namespace pl0::test
