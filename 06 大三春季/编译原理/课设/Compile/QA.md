| 问题 | 答案 |
| ---- | ---- |
| 虚拟机是怎么用栈实现的？ | `src/VM.cpp` 中的 `VirtualMachine` 维护一个可自动扩容的整型数组作为操作数栈：`push/pop/at` 封装入栈、出栈和随机访问；`Op::INT` 为当前栈帧开辟空间，`LOD/STO` 按静态链偏移加载或回写局部变量，`CAL/OPR` 负责过程调用与算术逻辑运算，所有指令围绕这一数组栈推进，实现典型的基于栈的解释器。 |
| P-Code 中的数字代表什么？ | 在 `include/pl0/PCode.hpp` 中，每条 `Instruction` 由 `op`、`level`、`argument` 构成：`op` 是操作码枚举（例如 `LOD`、`OPR`），`level` 用于静态链层差，`argument` 则表示立即数或操作子参数（如 `OPR` 对应的算术/逻辑操作编号、`LOD`/`STO` 的变量地址、`INT` 的栈帧大小等）。 |
| 数组是怎么实现的？ | 词法层识别 `[]`，语法阶段在 `Parser::parse_var_declarations()` 记录数组大小并写入 `Symbol` 的 `array_size`；代码生成的 `emit_array_access()`（`src/Codegen.cpp`）先用 `Op::LDA` 取基地址，再结合 `Op::CHK/IDX` 做越界检查与偏移计算，读写分别使用 `Op::LDI/STI`；虚拟机执行时在 `Op::CHK` 中检测上下界，保证安全访问。 |
| 各种复合符号是怎么实现的？ | 复合赋值与自增自减由 `Lexer::lex_symbol()`（`src/Lexer.cpp`）识别成独立 `TokenKind`，`Parser::parse_assignment()` 将其映射到 `AssignmentOperator` 枚举（`+=` 转为 `AddAssign`，`++` 转为 `AddAssign` + 常量 1）；代码生成时 `emit_assignment()` 解析该枚举，通过 `operation_for_assignment()` 选择对应 `Opr::ADD/SUB/MUL/DIV/MOD` 指令（数组赋值增加 `Op::DUP`/`Op::LDI` 读取旧值），最终在虚拟机中按普通算术指令执行。 |
