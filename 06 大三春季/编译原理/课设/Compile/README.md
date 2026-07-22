# PL/0编译程序的解析与改造

> 本项目用于燕山大学编译原理课程三级项目，仅供学习和参考，不可抄袭或商用
> 本项目仓库，只上传最纯净源码，请自行克隆并使用如下教程通过 CMake 进行编译和构建

---

## 一、项目环境

| 维度     | 建议配置         | 说明                                                         |
| -------- | ---------------- | ------------------------------------------------------------ |
| 操作系统 | MacOS 26.0.1      | CMake 工程、Qt 在三大平台都能工作，本项目以 MacOS 为主测试。 |
| CPU 架构 | ARM64            |                                                              |
| 编译器   | Apple Clang 17.0 |                                                              |
| C++ 标准 | C++20            | 使用 `std::span`、`std::variant`、`std::optional` 等现代语言特性。 |
| 构建系统 | CMake ≥ 3.20     | `cmake -S . -B build` 生成构建文件，`cmake --build build` 编译。 |
| GUI 依赖 | Qt5              |                                                              |
| 开发工具 | VSCode、iTerm    | VSCode主要编写代码，iTerm用于命令行调试                      |

前置准备（以 MacOS 为例）：

```bash
xcode-select --install               # 安装 Clang / make / lldb
brew install cmake ninja qt@5          # 安装构建工具与 Qt
```

---

## 二、项目构建

### 1. 基础 CMake 流水线

配置 & 编译：

```bash
# 生成 Debug 构建（默认开启 ASan/UBSan、CLI、GUI、测试）
cmake -S . -B build -G Ninja \
  -DPL0_BUILD_TESTS=ON \
  -DPL0_BUILD_TOOLS=ON \
  -DPL0_BUILD_GUI=ON

# 编译全部目标
cmake --build build
```

### 2. CMake 重要选项

| 选项                | 默认  | 作用                                               |
| ------------------- | ----- | -------------------------------------------------- |
| `PL0_BUILD_GUI`     | ON    | 控制是否编译 `gui/`。无 Qt 环境时设为 OFF 可跳过。 |
| `PL0_BUILD_TESTS`   | ON    | 启用 `tests/` 下的 Catch2 单元测试。               |
| `PL0_BUILD_TOOLS`   | ON    | 生成传统命令行工具 `pl0c`、`pl0run`、`pl0dis`。    |
| `PL0_ENABLE_ASAN`   | ON    | Debug 时自动加 `-fsanitize=address,undefined`。    |
| `CMAKE_BUILD_TYPE`  | Debug | 可切换为 Release：`-DCMAKE_BUILD_TYPE=Release`。   |
| `CMAKE_PREFIX_PATH` | —     | 指向 Qt 安装的 `lib/cmake` 目录，用于手动定位 Qt。 |

执行完后：

```
.
├── CMakeFiles
│   ├── 4.1.2
│   │   └── CompilerIdCXX
│   │       └── tmp
│   ├── CMakeScratch
│   ├── pkgRedirects
│   ├── pl0-cli.dir
│   │   └── src
│   ├── pl0-gui_autogen_timestamp_deps.dir
│   ├── pl0-gui_autogen.dir
│   ├── pl0-gui.dir
│   │   ├── gui
│   │   └── pl0-gui_autogen
│   ├── pl0.dir
│   │   └── src
│   ├── pl0c.dir
│   │   └── tools
│   ├── pl0dis.dir
│   │   └── tools
│   └── pl0run.dir
│       └── tools
├── pl0-gui_autogen
│   ├── DMHXEJ42XS
│   └── include
└── tests
    └── CMakeFiles
        ├── pl0_test_support.dir
        │   └── unit
        └── pl0_tests.dir
            └── unit

31 directories
```

- 静态库 `build/libpl0.a`
- CLI 前端 `build/pl0`
- 传统工具 `build/pl0c` / `build/pl0run` / `build/pl0dis`
- Qt GUI `build/pl0-gui`
- 单元测试 `build/tests/pl0_tests`



### 3. 常见构建组合

- **最小 CLI 环境**

  ```bash
  cmake -S . -B build-cli -DPL0_BUILD_GUI=OFF -DPL0_BUILD_TESTS=OFF
  cmake --build build-cli --target pl0 pl0c pl0run pl0dis
  ```

- **最小 GUI 环境**

  ```bash
  cmake -S . -B build-gui -DPL0_BUILD_TOOLS=OFF -DPL0_BUILD_TESTS=OFF
  cmake --build build-gui --target pl0-gui
  ```

- **Release**

  ```bash
  cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release
  cmake --build build-release --target pl0 pl0-gui
  strip build-release/pl0 build-release/pl0-gui   # 视平台可选
  ```


---

## 三、项目大纲

### 1. 顶层目录速览

```
.
├── CMakeLists.txt                # 顶层构建脚本
├── include/pl0/                  # 对外可复用的头文件
├── src/                          # 编译器 & 虚拟机核心实现
├── tools/                        # 单功能 CLI：pl0c/pl0run/pl0dis
├── gui/                          # Qt Widgets 图形前端
├── tests/                        # 单元测试 + 样例程序
├── docs/                         # 技术说明文档
├── pl0.c / pl0.h                 # 传统单文件实现（参考）
└── build/                        # CMake 生成的二进制产物（忽略可重建）
```

### 2. 模块关系（系统结构图）

```
┌──────────────┐
│  CLI (pl0)   │◄──┐
└──────────────┘   │                      ┌──────────────┐
┌──────────────┐   │   ┌────────────┐     │  VM (src/    │
│ Tools (c/run/│───┼──►│ libpl0.a   │────►│  VM.cpp)     │
│ dis)         │   │   │ (核心库)    │     └──────────────┘
└──────────────┘   │   │            │              │
┌──────────────┐   │   │            │      ┌───────▼───────┐
│ GUI (Qt)     │───┘   └────────────┘      │  Runtime I/O  │
└──────────────┘                           └—──────────────┘
```

核心库 `libpl0` 由以下子系统组成：

```
源代码 → Lexer → Token 流 → Parser → AST
                                  │
                                  ▼
                             CodeGenerator → P-Code → VirtualMachine → 输出
```


### 3. 数据流与文件角色

- `Lexer`（`src/Lexer.cpp`）：读取纯文本源码，生成带行列信息的 `Token`。
- `Parser`（`src/Parser.cpp`）：把 Token 串解析为 `AST`（抽象语法树）。
- `SymbolTable`（`src/SymbolTable.cpp`）：记录常量/变量/过程的作用域、地址、类型。
- `CodeGenerator`（`src/Codegen.cpp`）：遍历 AST，输出 P-Code 指令序列，同时导出符号表。
- `VirtualMachine`（`src/VM.cpp`）：执行 P-Code，负责运行时栈维护、I/O、越界检查。
- `Driver`（`src/Driver.cpp`）：把以上步骤封装成高层 API，CLI、GUI 均复用。

### 4. 逻辑/流程图

```
开始
 ↓
选择入口（GUI / CLI / 工具）
 ↓
读取源文件 (.pl0)
 ↓
compile_file()
  ↳ Lexer 词法分析
  ↳ Parser 语法 + 语义分析
  ↳ SymbolTable 建表
  ↳ CodeGenerator 生成 P-Code
  ↳ （可选）dump tokens/AST/symbols/pcode
 ↓
保存或直接传递 P-Code
 ↓
run_instructions()
  ↳ VirtualMachine 解释执行
  ↳ （可选）trace 指令、开启越界检查
 ↓
输出结果 / 诊断
 ↓
结束
```

---

## 四、项目功能

### GUI 用户图形化界面

1. **启动方式**

   ```bash
   ./build/pl0-gui
   ```


2. **主界面结构**
   - 左侧：`CodeEditor`（`gui/CodeEditor.cpp`）提供行号栏、关键字高亮风格。
   - 右侧 `QTabWidget`（`gui/MainWindow.cpp:226-258`）包含
     - **词法单元**：Token 类型、词素、行列、数值。
     - **语法树图**：可视化 AST 结构，自动绘制语法树。
     - **符号表**：名称、种类、层级、地址、数组尺寸。
     - **P-Code**：指令序列文本。
     - **诊断信息**：错误/警告分级显示。
     - **运行输出**：程序标准输出、`last_value`。
   - 底部：标准输入编辑器，可交互输入数据。
   - 右下角：水印标签与背景图片（`gui/MainWindow.cpp:508-560`）。

3. **功能按钮**
   - 打开、保存等基础功能
   - 编译、运行、编译并运行等核心功能，对应 `compileSource()` / `runProgram()` / `compileAndRun()`（`gui/MainWindow.cpp:602-680`）。
   - 选项勾选：
     - `启用数组越界检查` → 编译时加 `CHK` 指令。
     - `跟踪虚拟机指令` → 运行时在输出面板追加执行轨迹。

4. **AST 可视化**
  - `updateAstDiagram()`（`gui/MainWindow.cpp:321-403`）使用自定义树布局，将 AST 渲染成节点/连线图，适合课堂讲解。
  - 编译成功会自动导出语法树 PNG：若文件已有路径则输出到源文件目录，若是临时内容则写入系统图片目录下的 `PL0_AST/`，状态栏会提示完整导出路径。


### CLI 终端命令行界面

#### 1. **命令路径绑定（可选）**

   ```
export PATH="$PWD/build:$PATH"
pl0c tests/samples/assign_write.pl0
pl0run tests/samples/assign_write.pcode
   ```

   永久更改可写入 `~/.bash_profile` 或 `~/.zshrc`。




#### 2. `pl0c` 编译器

##### 2.1 使用场景

- **课程/实验评测**：将学生的 `.pl0` 程序批量编译为 P-Code，便于统一执行。
- **CLI 自动化**：结合 `make`/`ninja`/脚本进行持续集成测试。
- **调试**：配合 `--dump-*` 选项输出词法、语法树、符号表和 P-Code，追踪编译阶段细节。

##### 2.2 命令语法

```bash
pl0c <input.pl0> [-o out.pcode]
      [--dump-tokens --dump-ast --dump-sym --dump-pcode]
      [--bounds-check]
```

##### 2.3 选项说明

- `-o out.pcode`：自定义输出文件，默认与输入文件同名、扩展名 `.pcode`。
- `--dump-tokens`：在标准输出打印词法流（索引、类型、词素、取值）。
- `--dump-ast`：以缩进格式打印 AST 结构，便于核对语法分析。
- `--dump-sym`：在标准输出列出符号表信息（层级、地址、类型、传值方式）。
- `--dump-pcode`：实时打印生成的指令序列。
- `--bounds-check`：在代码生成阶段插入数组越界检查。

> 任意 `--dump-*` 输出均写入标准输出，可重定向至文件（例如 `pl0c foo.pl0 --dump-ast > foo.ast.txt`）。

##### 2.4 预期结果

- 编译成功：生成指定的 `.pcode` 文件，并返回退出码 0。
- 编译失败：将诊断信息写入标准错误，退出码为 1；不会创建或覆盖输出文件。


---

#### 3. `pl0run` 运行器

##### 3.1 使用场景

- **运行单个样例**：验证程序逻辑或课堂演示。
- **测试框架中的执行阶段**：将 `pl0c` 生成的 `.pcode` 进栈运行。
- **调试虚拟机**：配合 `--trace-vm` 查看指令执行轨迹与栈变化。

##### 3.2 命令语法

```bash
pl0run <input.pcode> [--trace-vm]
```

##### 3.3 选项说明

- `--trace-vm`：逐条打印 `opr`/`lod`/`sto` 等指令及重要寄存器状态，帮助分析运行流程。

##### 3.4 预期结果

- 成功：程序正常结束，退出码 0。若程序向标准输出写数据，控制台将直接显示。
- 失败：遇到运行时诊断（例如数组越界）、指令异常等时输出错误，并返回 1。


#### 4. `pl0dis` 反汇编器

##### 4.1 使用场景

- **调试代码生成**：验证 `pl0c` 生成的指令是否符合预期。
- **课堂讲解**：展示 P-Code 与 PL/0 源程序之间的对应关系。
- **版本对比**：将不同配置编译出的 `.pcode` 进行 diff。

##### 4.2 命令语法

```bash
pl0dis <input.pcode>
```

##### 4.3 预期结果

- 在标准输出打印每条指令（含序号、操作码、层差/地址/立即数等），结尾追加换行。
- 读取失败或文件格式错误将导致退出码 1。



## 五、核心代码

| 模块        | 源文件                        | 关注点                                        | 说明                                                         |
| ----------- | ----------------------------- | --------------------------------------------- | ------------------------------------------------------------ |
| 词法分析    | `src/Lexer.cpp:20-203`        | `Lexer::next()`、注释/布尔/数组语法           | 支持 `//`、`/* … */`、布尔字面量、新运算符。`DiagnosticSink` 用于错误报告。 |
| 语法分析    | `src/Parser.cpp:52-225`       | 递归下降 + Panic 模式同步                     | `parse_if`、`parse_repeat` 等新语句；`synchronize()` 根据 FIRST/FOLLOW 恢复。 |
| 抽象语法树  | `include/pl0/AST.hpp`         | `Expression`/`Statement` 变体                 | 使用 `std::variant` 表达不同节点，辅以 `unique_ptr` 管理所有权。 |
| 符号表      | `src/SymbolTable.cpp`         | 层次作用域 + 地址分配                         | `enter_scope()`/`leave_scope()` 维护静态链深度与局部变量偏移。 |
| 代码生成    | `src/Codegen.cpp:33-212`      | LDA/LDI/STI/CHK/DUP、If/While/Repeat、复合赋值 | 支持数组越界检查、布尔/逻辑运算、`repeat` 后测循环；复合赋值根据操作符生成对应算术；过程尚未加入参数。 |
| P-Code 表达 | `include/pl0/PCode.hpp`       | 新指令 LDA/IDX/CHK/DUP                        | 扩展原 PL/0 指令集以支持数组、越界检查与地址复用。           |
| 虚拟机      | `src/VM.cpp:19-210`           | 栈式执行、调试追踪、防护                      | 自动扩容栈、检测除零/越界、记录 `last_value`。               |
| 诊断系统    | `include/pl0/Diagnostics.hpp` | 错误分级、打印格式                            | CLI 与 GUI 共享 `print_diagnostics()`，确保消息一致。        |
| 高层封装    | `src/Driver.cpp:145-227`      | `compile_source_text()`、`run_instructions()` | 统一入口，支持 token/AST/符号/P-Code dump，与 GUI 共享。     |
| CLI 主程序  | `src/main.cpp:19-171`         | 子命令派发、参数解析                          | 单一可执行 `pl0` 即可覆盖编译/运行/反汇编。                  |
| GUI 主窗口  | `gui/MainWindow.cpp:70-403`   | AST 绘制、面板填充、工具栏                    | 编译结果存入 `lastResult_`，运行时重定向 `std::cout`/`std::cin`。 |

> 建议在阅读代码时配合 `tests/unit/*.cpp`，那里提供针对性的输入 → 输出断言，印证每个阶段的行为。

---

## 六、语法扩展与实现

### 1. 词法与记号
- `src/Lexer.cpp` 通过指针推进和手写状态机解析空白、行注释 `//`、块注释 `/*…*/`，并借助 `std::from_chars` 读取整数字面量。
- 关键字映射集中在 `src/Symbol.cpp` 的 `keyword_table()`，最终落在 `TokenKind`（`include/pl0/Token.hpp`）枚举中。
- 复合赋值与自增/自减在 `Lexer::lex_symbol()` 中以多字符匹配实现，当前支持 `+= -= *= /= %= ++ --`；对应的 `TokenKind` 与 `to_string()` 均已更新。

### 2. 声明语法
- 程序结构：`Program → Block '.'`，由 `Parser::parse_program()`（`src/Parser.cpp`）实现。
- 常量声明：`const` 列表允许数字或布尔字面量；常量值写入 `Symbol::constant_value`，代码生成阶段直接使用 `Op::LIT`。
- 变量/数组：`var` 语句支持 `name` 或 `name[整型常量]`；数组容量存入 `Symbol::size`，`emit_var()` 为其分配静态偏移。
- 过程声明：`procedure name; Block;` 采用两阶段策略，先注册符号再生成指令，支持前向与递归调用（参数尚未开放）。

### 3. 语句语法
- 语句分派：`Parser::parse_statement()` 覆盖赋值、调用、`begin...end`、`if/else`、`while`、`repeat/until`、`read`、`write/writeln`。
- 赋值：`AssignmentStmt` 新增 `AssignmentOperator` 枚举；`x += y` 等价于 `x := x + y`。数组下标赋值 `a[i] += v` 通过地址加载、`Op::DUP`、`Op::LDI` 组合完成读改写。
- 条件与循环：`emit_if()`、`emit_while()`、`emit_repeat()` 分别利用 `Op::JPC` / `Op::JMP` 构造控制流；`repeat` 为后测循环。
- I/O：`read` 语句调用 `Opr::READ`，`write`/`writeln` 对应 `Opr::WRITE`/`Opr::WRITELN`，所有读写使用 `DiagnosticSink` 汇报错误。

### 4. 表达式语法（自低到高）
- `Expression` 支持布尔短路逻辑 (`or`)、`LogicTerm` 处理 `and`，逻辑非 `not` 在 `parse_logic_factor()` 中作为一元运算。
- 比较运算 `= # < <= > >=` 产生 `BinaryOp`；算术部分提供 `+ - * / %`，均映射为虚拟机 `Opr` 操作。
- 一元运算包含 `+ - not odd`，其中 `odd` 直接调用 `Opr::ODD` 测试奇偶。
- 基本因子：整型/布尔字面量、标识符、数组访问、括号表达式。数组访问由 `emit_array_access()` 统一处理，可选越界检查 `Op::CHK`。

### 5. P-Code 与运行期
- 指令枚举 `include/pl0/PCode.hpp` 在传统 PL/0 基础上扩展 `LDA/IDX/LDI/STI/CHK/DUP`，覆盖数组寻址、越界检查与复合赋值所需的地址复制。
- `CodeGenerator` 使用访问器模式（`std::visit` + `Overloaded`）生成指令序列；`operation_for_assignment()` 根据 `AssignmentOperator` 选择 `Opr::ADD/SUB/MUL/DIV/MOD`。
- `VirtualMachine` (`src/VM.cpp`) 采用自动扩容的数组栈。`Op::DUP` 会复制栈顶值；`Op::CHK` 在越界时经 `DiagnosticSink` 报错后终止执行。

### 6. 调试与可视化
- CLI 通过 `--dump-tokens/--dump-ast/--dump-sym/--dump-pcode` 输出各阶段快照，函数集中在 `src/Driver.cpp`。
- Qt GUI 的 AST、符号表、指令面板复用同一数据结构；`assignmentOpName()` 在界面上显示 `+=` 等新操作。

### 7. 测试覆盖
- `tests/unit/LexerTests.cpp` 检查复合赋值、自增自减等新记号。
- `tests/unit/ParserTests.cpp` 验证 `AssignmentOperator` 枚举、字面量降级与语句列表解析。
- `tests/unit/CodegenTests.cpp` 对复合赋值、数组复合赋值及 `Op::DUP`/`Op::LDI` 插入进行断言。
- `tests/unit/VmTests.cpp` 运行实际程序，确认虚拟机对 `+= -= *= /= %= ++ --` 的算术语义。

### 8. 语法特性与示例映射
| 语法特性 | 示例程序 | 实现要点 |
| -------- | -------- | -------- |
| 自增 / 自减 `++ --` | `tests/samples/compound_assign.pl0` | 词法在 `src/Lexer.cpp:200` 扩展多字符记号；递归下降于 `src/Parser.cpp:218` 将其转换为 `AssignmentOperator::Add/SubAssign`；`src/Codegen.cpp:135` 生成 `LOD` + `Opr::ADD/SUB` 序列。 |
| 复合赋值 `+= -= *= /= %=` | `tests/samples/compound_assign.pl0`、`tests/samples/while_arith.pl0` | 与上相同的词法/语法管道，代码生成在 `src/Codegen.cpp:149` 分支调用 `operation_for_assignment()` 并发射对应算术操作。 |
| 数组读写与越界循环 | `tests/samples/array_bounds.pl0` | `Lexer` 识别 `[]`，`Parser::parse_var_declarations()` 注册大小；`CodeGenerator::emit_array_access()` 使用 `Op::LDA/CHK/IDX/LDI`；虚拟机的 `Op::CHK` 位于 `src/VM.cpp:251`。 |
| 布尔逻辑 `and/or/not` | `tests/samples/if_else.pl0` | `Parser::parse_expression()` / `parse_logic_term()` 组合出 `BinaryOp::And/Or` 与 `UnaryOp::Not`；代码生成后在 `src/Codegen.cpp:172` 发射逻辑 `Opr`。 |
| `repeat ... until` 后测循环 | `tests/samples/repeat_until.pl0` | `Parser::parse_repeat()` 构造 `RepeatStmt`，`CodeGenerator::emit_repeat()` 负责回跳；虚拟机 `Op::JPC` 控制退出。 |
| 多目标 `read` / `write` | `tests/samples/io_mix.pl0` | `parse_read()` 支持括号内多标识符；`parse_write()` 兼容多表达式输出，运行时由 `Opr::READ/WRITE`（`src/VM.cpp:129` 起）完成。 |

### 9. 示例批量测试脚本
- 执行 `python tools/run_samples.py`（或直接运行脚本）即可依次编译、运行 `tests/samples/*.pl0`，并将源代码、`pl0c` 反汇编结果与运行输出统一写入 `tests/sample_report.txt`，方便课堂演示或回归验证。

---

## 七、目标实现

| 号   | 评分条目                           | 完成度 & 证据                                                |
| ---- | ---------------------------------- | ------------------------------------------------------------ |
| 1-1  | **总体结构、头文件、数据结构理解** | 项目已模块化成 `include/pl0/*.hpp` + `src/*.cpp`，每个阶段独立；`docs/ProjectStructure.md` 与本文“大纲”章节全面说明。 |
| 1-2  | **词法分析熟悉程度**               | `src/Lexer.cpp` 支持关键字表（`pl0/Symbol.hpp`）、注释、布尔、数组；`--dump-tokens`、GUI 词法表便于演示。 |
| 1-3  | **语法语义分析理解**               | `src/Parser.cpp` 扩展 `if...else`、`repeat...until`，采用错误恢复；GUI AST 图和 `--dump-ast` 可视化语法层级。 |
| 1-4  | **目标代码结构 & 生成**            | `src/Codegen.cpp` 输出扩展 P-Code；`--dump-pcode` 与 `pl0dis` 直观展示；数组越界检查使用 `CHK`。 |
| 1-5  | **错误处理机制**                   | `DiagnosticSink` 全程收集；CLI `print_diagnostics()`、GUI Diagnostics 面板统一输出，覆盖词法/语法/运行期。 |
| 1-6  | **运行时存储分配与解释执行**       | `src/VM.cpp` 栈式虚拟机实现静态链、`INT` 分配、`base()` 回溯；`--trace-vm` 展示执行轨迹。 |
| 2    | **改进与技术报告**                 | 新增语法（布尔、数组、repeat）、P-Code 指令（LDA/LDI/STI/CHK）、越界检查、GUI 可视化、ASan、单元测试、文档三件套；`tests/samples/*.pl0` 与 `tests/unit` 佐证改进可通过测试。 |
| 3    | **考核展示/演讲**                  | Qt GUI 支持全功能演示；CLI + 测试可展示自动化；文档化报告（本文件 + README + ToolingGuide）满足 OBE “成果可展示、过程可追溯”。 |

---

## 八、反思总结

1. **尚未实现/可改进项**
   - 过程参数、值/引用传递尚未落地（`CodeGenerator::emit_call()` 当前直接报错）。
   - 编译器未自举（仍以 C++ 实现），可尝试用 PL/0 复刻子集后自编译。
   - 诊断信息暂未关联源码高亮（GUI 仅文本列表），可引入文本标记或跳转。
   - 虚拟机仅支持整数类型，布尔值以 0/1 表示；未来可扩展为枚举或记录类型。
   - 测试覆盖率主要集中在单元层，缺少长程序或性能回归脚本。

2. **未来展望**
   - 引入 **过程参数 & 调用约定**，完善教学案例。
   - 尝试实现 **P-Code 优化**（常量折叠、死代码删除）并提供可视化对比。
   - 在 GUI 中整合 **断点调试/单步执行**，辅助讲解运行时栈。
   - 编写 **自动评分脚本**（结合 `pl0c` + `pl0run` + `diff`），方便实验班批改。
   - 构建 **在线版演示**（例如使用 Qt for WebAssembly 或 CLI + Web 前端），便于比赛/答辩展示。
