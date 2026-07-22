// 文件: PCode.cpp
// 功能: 实现 P-Code 指令序列的辅助函数
#include "pl0/PCode.hpp"

#include <cctype>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

namespace pl0 {

// 函数: 将操作码转为文本
std::string to_string(Op op) {
  switch (op) {
    case Op::LIT:
      return "lit";
    case Op::OPR:
      return "opr";
    case Op::LOD:
      return "lod";
    case Op::STO:
      return "sto";
    case Op::CAL:
      return "cal";
    case Op::INT:
      return "int";
    case Op::JMP:
      return "jmp";
    case Op::JPC:
      return "jpc";
    case Op::LDA:
      return "lda";
    case Op::IDX:
      return "idx";
    case Op::LDI:
      return "ldi";
    case Op::STI:
      return "sti";
    case Op::CHK:
      return "chk";
    case Op::DUP:
      return "dup";
    case Op::NOP:
      return "nop";
  }
  return "unknown";
}

// 函数: 将 OPR 子操作码转为文本
std::string to_string(Opr opr) {
  switch (opr) {
    case Opr::RET:
      return "ret";
    case Opr::NEG:
      return "neg";
    case Opr::ADD:
      return "add";
    case Opr::SUB:
      return "sub";
    case Opr::MUL:
      return "mul";
    case Opr::DIV:
      return "div";
    case Opr::ODD:
      return "odd";
    case Opr::MOD:
      return "mod";
    case Opr::EQ:
      return "eq";
    case Opr::NE:
      return "ne";
    case Opr::LT:
      return "lt";
    case Opr::GE:
      return "ge";
    case Opr::GT:
      return "gt";
    case Opr::LE:
      return "le";
    case Opr::WRITE:
      return "write";
    case Opr::WRITELN:
      return "writeln";
    case Opr::READ:
      return "read";
    case Opr::AND:
      return "and";
    case Opr::OR:
      return "or";
    case Opr::NOT:
      return "not";
  }
  return "unknown";
}

// 函数: 文字化单条指令
std::string to_string(const Instruction& instr) {
  std::ostringstream oss;
  oss << to_string(instr.op) << " " << instr.level << " ";
  if (instr.op == Op::OPR) {
    oss << to_string(static_cast<Opr>(instr.argument));
  } else {
    oss << instr.argument;
  }
  return oss.str();
}

// 函数: 从文本解析出指令
Instruction parse_instruction(const std::string& text) {
  std::istringstream iss(text);
  std::string op_text;
  Instruction instr;
  if (!(iss >> op_text)) {
    throw std::runtime_error("empty instruction");
  }

  auto normalize = [](std::string value) {
    for (auto& ch : value) {
      ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    }
    return value;
  };

  const std::unordered_map<std::string, Op> op_map{
      {"lit", Op::LIT}, {"opr", Op::OPR}, {"lod", Op::LOD}, {"sto", Op::STO},
      {"cal", Op::CAL}, {"int", Op::INT}, {"jmp", Op::JMP}, {"jpc", Op::JPC},
      {"lda", Op::LDA}, {"idx", Op::IDX}, {"ldi", Op::LDI}, {"sti", Op::STI},
      {"chk", Op::CHK}, {"dup", Op::DUP}, {"nop", Op::NOP},
  };

  auto op_it = op_map.find(normalize(op_text));
  if (op_it == op_map.end()) {
    throw std::runtime_error("unknown opcode: " + op_text);
  }
  instr.op = op_it->second;
  if (!(iss >> instr.level)) {
    throw std::runtime_error("missing level");
  }
  if (instr.op == Op::OPR) {
    std::string opr_text;
    if (!(iss >> opr_text)) {
      throw std::runtime_error("expected opr mnemonic");
    }
    const std::unordered_map<std::string, Opr> opr_map{
        {"ret", Opr::RET},     {"neg", Opr::NEG},     {"add", Opr::ADD},
        {"sub", Opr::SUB},     {"mul", Opr::MUL},     {"div", Opr::DIV},
        {"odd", Opr::ODD},     {"mod", Opr::MOD},     {"eq", Opr::EQ},
        {"ne", Opr::NE},       {"lt", Opr::LT},       {"ge", Opr::GE},
        {"gt", Opr::GT},       {"le", Opr::LE},       {"write", Opr::WRITE},
        {"writeln", Opr::WRITELN}, {"read", Opr::READ}, {"and", Opr::AND},
        {"or", Opr::OR},       {"not", Opr::NOT},
    };
    auto opr_it = opr_map.find(normalize(opr_text));
    if (opr_it == opr_map.end()) {
      throw std::runtime_error("unknown opr mnemonic: " + opr_text);
    }
    instr.argument = static_cast<std::int32_t>(opr_it->second);
  } else {
    if (!(iss >> instr.argument)) {
      throw std::runtime_error("missing argument");
    }
  }
  return instr;
}

// 函数: 将指令序列写入流
void serialize_instructions(const InstructionSequence& instructions,
                            std::ostream& out) {
  for (std::size_t i = 0; i < instructions.size(); ++i) {
    const auto& instr = instructions[i];
    out << std::setw(4) << i << ": " << to_string(instr);
    if (i + 1 < instructions.size()) {
      out << '\n';
    }
  }
}

// 函数: 从流中读取指令序列
InstructionSequence deserialize_instructions(std::istream& in) {
  InstructionSequence instructions;
  std::string line;
  while (std::getline(in, line)) {
    if (line.empty()) {
      continue;
    }
    auto colon = line.find(':');
    if (colon != std::string::npos) {
      line = line.substr(colon + 1);
    }
    // trim leading spaces
    auto pos = line.find_first_not_of(" \t");
    if (pos != std::string::npos) {
      line = line.substr(pos);
    }
    instructions.push_back(parse_instruction(line));
  }
  return instructions;
}

}  // namespace pl0
