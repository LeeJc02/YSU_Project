// 文件: VM.hpp
// 功能: 声明基于栈的 P-Code 虚拟机
#pragma once

#include <cstdint>
#include <vector>

#include "pl0/Diagnostics.hpp"
#include "pl0/Options.hpp"
#include "pl0/PCode.hpp"

namespace pl0 {

// 类: 执行 P-Code 指令的虚拟机
class VirtualMachine {
 public:
  // 结构: 运行结果描述
  struct Result {
    bool success = true;
    std::int64_t last_value = 0;
  };

  // 构造: 绑定诊断与运行选项
  VirtualMachine(DiagnosticSink& diagnostics, const RunnerOptions& options);

  // 函数: 执行指令序列
  Result execute(const InstructionSequence& code);

 private:
  // 工具: 栈操作与静态链定位
  void push(std::int64_t value);
  std::int64_t pop();
  std::int64_t& at(int index);
  int base(int level, int b) const;

  // 成员: 运行时状态
  DiagnosticSink& diagnostics_;
  const RunnerOptions& options_;
  std::vector<std::int64_t> stack_;
  int stack_top_ = 0;
  int base_pointer_ = 0;
  int program_counter_ = 0;
};

}  // namespace pl0
