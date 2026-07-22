// 文件: PCode.hpp
// 功能: 定义 P-Code 指令集及读写工具
#pragma once

#include <cstdint>
#include <ostream>
#include <string>
#include <string_view>
#include <vector>

namespace pl0 {

// 枚举: 指令操作码
enum class Op : std::uint8_t {
  LIT,
  OPR,
  LOD,
  STO,
  CAL,
  INT,
  JMP,
  JPC,
  LDA,
  IDX,
  LDI,
  STI,
  CHK,
  DUP,
  NOP,
};

// 枚举: OPR 子操作码
enum class Opr : std::uint8_t {
  RET = 0,
  NEG = 1,
  ADD = 2,
  SUB = 3,
  MUL = 4,
  DIV = 5,
  ODD = 6,
  MOD = 7,
  EQ = 8,
  NE = 9,
  LT = 10,
  GE = 11,
  GT = 12,
  LE = 13,
  WRITE = 14,
  WRITELN = 15,
  READ = 16,
  AND = 17,
  OR = 18,
  NOT = 19,
};

// 结构: 指令实体
struct Instruction {
  Op op = Op::NOP;
  std::int32_t level = 0;
  std::int32_t argument = 0;
};

using InstructionSequence = std::vector<Instruction>;

// 函数: 指令/操作码转字符串
std::string to_string(Op op);
std::string to_string(Opr opr);
std::string to_string(const Instruction& instr);

// 函数: 文本解析为指令
Instruction parse_instruction(const std::string& text);

// 函数: 序列化/反序列化指令流
void serialize_instructions(const InstructionSequence& instructions,
                            std::ostream& out);

InstructionSequence deserialize_instructions(std::istream& in);

}  // namespace pl0
