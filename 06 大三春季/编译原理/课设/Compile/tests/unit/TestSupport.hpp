#pragma once

#include <string_view>

#include "pl0/Codegen.hpp"
#include "pl0/Diagnostics.hpp"

namespace pl0::test {

InstructionSequence compile_source(std::string_view source,
                                   const CompilerOptions& options,
                                   DiagnosticSink& diagnostics);

void initialize_environment();

}  // namespace pl0::test
