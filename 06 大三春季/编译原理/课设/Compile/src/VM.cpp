// 文件: VM.cpp
// 功能: 实现基于栈的 P-Code 虚拟机
#include "pl0/VM.hpp"

#include <iostream>
#include <stdexcept>

namespace pl0 {

namespace {

// 常量: 初始栈容量
constexpr std::size_t kInitialStackSize = 1024;

}  // namespace

// 构造: 记录诊断器与运行时选项
VirtualMachine::VirtualMachine(DiagnosticSink& diagnostics,
                               const RunnerOptions& options)
    : diagnostics_(diagnostics), options_(options) {}

// 函数: 执行指令序列并返回运行结果
VirtualMachine::Result VirtualMachine::execute(const InstructionSequence& code) {
  Result result;
  stack_.assign(kInitialStackSize, 0);
  stack_top_ = 0;
  base_pointer_ = 0;
  program_counter_ = 0;
  stack_[0] = stack_[1] = stack_[2] = 0;

  auto ensure_capacity = [&](int index) {
    if (index >= static_cast<int>(stack_.size())) {
      stack_.resize(static_cast<std::size_t>(index) + 1024, 0);
    }
  };

  try {
    while (program_counter_ >= 0 &&
           program_counter_ < static_cast<int>(code.size())) {
      const Instruction instr =
          code[static_cast<std::size_t>(program_counter_++)];

      if (options_.trace_vm) {
        std::cout << program_counter_ - 1 << ": " << to_string(instr) << '\n';
      }

      switch (instr.op) {
      case Op::LIT:
        push(instr.argument);
        break;
      case Op::OPR: {
        auto operation = static_cast<Opr>(instr.argument);
        switch (operation) {
          case Opr::RET: {
            int old_base = base_pointer_;
            int return_addr = static_cast<int>(at(base_pointer_ + 2));
            base_pointer_ = static_cast<int>(at(base_pointer_ + 1));
            stack_top_ = old_base;
            program_counter_ = return_addr;
            if (base_pointer_ == 0 && program_counter_ == 0) {
              return result;
            }
            break;
          }
          case Opr::NEG: {
            at(stack_top_ - 1) = -at(stack_top_ - 1);
            break;
          }
          case Opr::ADD: {
            auto rhs = pop();
            auto lhs = pop();
            push(lhs + rhs);
            result.last_value = stack_top_ > 0 ? at(stack_top_ - 1) : 0;
            break;
          }
          case Opr::SUB: {
            auto rhs = pop();
            auto lhs = pop();
            push(lhs - rhs);
            result.last_value = stack_top_ > 0 ? at(stack_top_ - 1) : 0;
            break;
          }
          case Opr::MUL: {
            auto rhs = pop();
            auto lhs = pop();
            push(lhs * rhs);
            result.last_value = stack_top_ > 0 ? at(stack_top_ - 1) : 0;
            break;
          }
          case Opr::DIV: {
            auto rhs = pop();
            if (rhs == 0) {
              diagnostics_.report({DiagnosticLevel::Error, DiagnosticCode::DivisionByZero,
                                   "division by zero", {}});
              result.success = false;
              return result;
            }
            auto lhs = pop();
            push(lhs / rhs);
            result.last_value = stack_top_ > 0 ? at(stack_top_ - 1) : 0;
            break;
          }
          case Opr::ODD: {
            auto value = pop();
            push(value % 2 != 0 ? 1 : 0);
            break;
          }
          case Opr::MOD: {
            auto rhs = pop();
            if (rhs == 0) {
              diagnostics_.report({DiagnosticLevel::Error, DiagnosticCode::DivisionByZero,
                                   "modulo by zero", {}});
              result.success = false;
              return result;
            }
            auto lhs = pop();
            push(lhs % rhs);
            result.last_value = stack_top_ > 0 ? at(stack_top_ - 1) : 0;
            break;
          }
          case Opr::EQ: {
            auto rhs = pop();
            auto lhs = pop();
            push(lhs == rhs ? 1 : 0);
            break;
          }
          case Opr::NE: {
            auto rhs = pop();
            auto lhs = pop();
            push(lhs != rhs ? 1 : 0);
            break;
          }
          case Opr::LT: {
            auto rhs = pop();
            auto lhs = pop();
            push(lhs < rhs ? 1 : 0);
            break;
          }
          case Opr::GE: {
            auto rhs = pop();
            auto lhs = pop();
            push(lhs >= rhs ? 1 : 0);
            break;
          }
          case Opr::GT: {
            auto rhs = pop();
            auto lhs = pop();
            push(lhs > rhs ? 1 : 0);
            break;
          }
          case Opr::LE: {
            auto rhs = pop();
            auto lhs = pop();
            push(lhs <= rhs ? 1 : 0);
            break;
          }
          case Opr::WRITE: {
            auto value = pop();
            std::cout << value;
            result.last_value = value;
            break;
          }
          case Opr::WRITELN: {
            std::cout << '\n';
            break;
          }
          case Opr::READ: {
            std::int64_t value = 0;
            std::cin >> value;
            push(value);
            break;
          }
          case Opr::AND: {
            auto rhs = pop();
            auto lhs = pop();
            push((lhs != 0 && rhs != 0) ? 1 : 0);
            break;
          }
          case Opr::OR: {
            auto rhs = pop();
            auto lhs = pop();
            push((lhs != 0 || rhs != 0) ? 1 : 0);
            break;
          }
          case Opr::NOT: {
            auto value = pop();
            push(value == 0 ? 1 : 0);
            break;
          }
        }
        break;
      }
      case Op::LOD: {
        ensure_capacity(base(instr.level, base_pointer_) + instr.argument + 1);
        push(at(base(instr.level, base_pointer_) + instr.argument));
        break;
      }
      case Op::STO: {
        auto value = pop();
        ensure_capacity(base(instr.level, base_pointer_) + instr.argument + 1);
        at(base(instr.level, base_pointer_) + instr.argument) = value;
        break;
      }
      case Op::CAL: {
        ensure_capacity(stack_top_ + 3);
        at(stack_top_) = base(instr.level, base_pointer_);
        at(stack_top_ + 1) = base_pointer_;
        at(stack_top_ + 2) = program_counter_;
        base_pointer_ = stack_top_;
        program_counter_ = instr.argument;
        break;
      }
      case Op::INT: {
        ensure_capacity(stack_top_ + instr.argument);
        for (int i = 0; i < instr.argument; ++i) {
          at(stack_top_ + i) = 0;
        }
        stack_top_ += instr.argument;
        break;
      }
      case Op::JMP: {
        program_counter_ = instr.argument;
        break;
      }
      case Op::JPC: {
        auto value = pop();
        if (value == 0) {
          program_counter_ = instr.argument;
        }
        break;
      }
      case Op::LDA: {
        ensure_capacity(base(instr.level, base_pointer_) + instr.argument + 1);
        push(base(instr.level, base_pointer_) + instr.argument);
        break;
      }
      case Op::IDX: {
        auto index = pop();
        auto address = pop();
        push(address + index);
        break;
      }
      case Op::LDI: {
        auto address = pop();
        ensure_capacity(static_cast<int>(address) + 1);
        push(at(static_cast<int>(address)));
        break;
      }
      case Op::STI: {
        auto value = pop();
        auto address = pop();
        ensure_capacity(static_cast<int>(address) + 1);
        at(static_cast<int>(address)) = value;
        break;
      }
      case Op::CHK: {
        auto index = pop();
        if (index < 0 || index >= instr.argument) {
          diagnostics_.report({DiagnosticLevel::Error, DiagnosticCode::InvalidArraySubscript,
                               "array index out of bounds", {}});
          result.success = false;
          return result;
        }
        push(index);
        break;
      }
      case Op::DUP: {
        if (stack_top_ == 0) {
          diagnostics_.report({DiagnosticLevel::Error, DiagnosticCode::StackUnderflow,
                               "stack underflow on dup", {}});
          result.success = false;
          return result;
        }
        auto value = at(stack_top_ - 1);
        push(value);
        break;
      }
      case Op::NOP:
        break;
      }
    }
  } catch (const std::exception& ex) {
    diagnostics_.report({DiagnosticLevel::Error, DiagnosticCode::RuntimeError,
                         ex.what(), {}});
    result.success = false;
  }

  return result;
}

// 函数: 向栈压入一个值
void VirtualMachine::push(std::int64_t value) {
  if (stack_top_ >= static_cast<int>(stack_.size())) {
    stack_.resize(static_cast<std::size_t>(stack_top_) + 1024, 0);
  }
  stack_[static_cast<std::size_t>(stack_top_++)] = value;
}

// 函数: 弹出栈顶值
std::int64_t VirtualMachine::pop() {
  if (stack_top_ == 0) {
    throw std::runtime_error("stack underflow");
  }
  return stack_[static_cast<std::size_t>(--stack_top_)];
}

// 函数: 访问指定栈下标
std::int64_t& VirtualMachine::at(int index) {
  if (index < 0) {
    throw std::runtime_error("negative stack access");
  }
  if (index >= static_cast<int>(stack_.size())) {
    stack_.resize(static_cast<std::size_t>(index) + 1024, 0);
  }
  return stack_[static_cast<std::size_t>(index)];
}

// 函数: 按静态链回溯基地址
int VirtualMachine::base(int level, int b) const {
  int result = b;
  while (level > 0) {
    result = static_cast<int>(stack_[static_cast<std::size_t>(result)]);
    level--;
  }
  return result;
}

}  // namespace pl0
