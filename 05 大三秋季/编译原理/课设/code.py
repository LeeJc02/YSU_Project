import sys
from dataclasses import dataclass
from pathlib import Path
from typing import List, Optional


from PyQt5.QtCore import Qt, QTimer
from PyQt5.QtGui import QIcon, QTextOption
from PyQt5.QtWidgets import (
        QApplication,
        QFileDialog,
        QFrame,
        QHBoxLayout,
        QLabel,
        QLineEdit,
        QMainWindow,
        QPlainTextEdit,
        QPushButton,
        QSizePolicy,
        QSplitter,
        QMessageBox,
        QVBoxLayout,
        QWidget,
    )


class LexerError(Exception):
    pass


class ParserError(Exception):
    def __init__(self, message: str, position: int, found: str, expected: List[str]):
        super().__init__(message)
        self.position = position
        self.found = found
        self.expected = expected


@dataclass
class Token:
    type: str
    value: str
    position: int


@dataclass
class Node:
    label: str
    value: Optional[str] = None
    children: Optional[List["Node"]] = None

    def __post_init__(self):
        if self.children is None:
            self.children = []


def tokenize(text: str) -> List[Token]:
    # 词法分析：扫描表达式字符串并切分为 Token 列表
    tokens: List[Token] = []
    i = 0
    length = len(text)
    while i < length:
        ch = text[i]
        if ch.isspace():
            i += 1
            continue
        if ch.isdigit():
            start = i
            while i < length and text[i].isdigit():
                i += 1
            tokens.append(Token("NUM", text[start:i], start))
            continue
        if ch.isalpha() or ch == "_":
            start = i
            i += 1
            while i < length and (text[i].isalnum() or text[i] == "_"):
                i += 1
            tokens.append(Token("ID", text[start:i], start))
            continue
        if ch == "+":
            if i + 1 < length and text[i + 1] == "+":
                tokens.append(Token("INCREMENT", "++", i))
                i += 2
                continue
            tokens.append(Token("PLUS", ch, i))
            i += 1
            continue
        if ch == "-":
            if i + 1 < length and text[i + 1] == "-":
                tokens.append(Token("DECREMENT", "--", i))
                i += 2
                continue
            tokens.append(Token("MINUS", ch, i))
            i += 1
            continue
        if ch == "/":
            if i + 1 < length and text[i + 1] == "/":
                i += 2
                while i < length and text[i] not in {"\n", "\r"}:
                    i += 1
                continue
            tokens.append(Token("DIV", ch, i))
            i += 1
            continue
        single_char_tokens = {
            "*": "MUL",
            "(": "LPAREN",
            ")": "RPAREN",
        }
        if ch in single_char_tokens:
            tokens.append(Token(single_char_tokens[ch], ch, i))
            i += 1
            continue
        raise LexerError(f"无法识别的字符 '{ch}' (位置 {i})")
    tokens.append(Token("EOF", "", length))
    return tokens


class Parser:
    NONTERMINALS = {"E", "E'", "T", "T'", "F", "P", "S"}

    def __init__(self, tokens: List[Token]):
        # 语法分析：保存 token 流、当前位置，以及最左推导状态
        self.tokens = tokens
        self.index = 0
        self.derivation_steps: List[str] = ["E"]
        self.current_sentential_form: List[str] = ["E"]

    def current(self) -> Token:
        return self.tokens[self.index]

    def advance(self) -> Token:
        token = self.tokens[self.index]
        self.index += 1
        return token

    def consume(self, token_type: str, expected_desc: Optional[str] = None) -> Token:
        token = self.current()
        if token.type == token_type:
            return self.advance()
        expected = expected_desc or token_type
        raise ParserError(
            message=f"位置 {token.position} 处需要 {expected}",
            position=token.position,
            found=token.type,
            expected=[expected],
        )

    def record(self, lhs: str, rhs: List[str]):
        # 记录一次产生式替换，便于输出最左推导序列
        try:
            idx = next(i for i, sym in enumerate(self.current_sentential_form) if sym == lhs)
        except StopIteration:
            return
        if rhs:
            self.current_sentential_form = (
                self.current_sentential_form[:idx] + rhs + self.current_sentential_form[idx + 1 :]
            )
        else:
            self.current_sentential_form = self.current_sentential_form[:idx] + self.current_sentential_form[idx + 1 :]
        step = " ".join(self.current_sentential_form) if self.current_sentential_form else "ε"
        self.derivation_steps.append(step)

    def parse(self) -> Node:
        # 从非终结符 E 开始递归下降
        tree = self.parse_E()
        if self.current().type != "EOF":
            token = self.current()
            raise ParserError(
                message=f"多余符号 '{token.value}'", position=token.position, found=token.type, expected=["EOF"]
            )
        return tree

    def parse_E(self) -> Node:
        self.record("E", ["T", "E'"])
        left = self.parse_T()
        right = self.parse_Ep()
        return Node("E", children=[left, right])

    def parse_Ep(self) -> Node:
        token = self.current()
        if token.type == "PLUS":
            self.record("E'", ["+", "T", "E'"])
            op = self.consume("PLUS")
            t_node = self.parse_T()
            ep_node = self.parse_Ep()
            return Node("E'", children=[Node("op", value=op.value), t_node, ep_node])
        if token.type == "MINUS":
            self.record("E'", ["-", "T", "E'"])
            op = self.consume("MINUS")
            t_node = self.parse_T()
            ep_node = self.parse_Ep()
            return Node("E'", children=[Node("op", value=op.value), t_node, ep_node])
        if token.type in {"RPAREN", "EOF"}:
            self.record("E'", [])
            return Node("E'", value="ε")
        raise ParserError(
            message=f"位置 {token.position} 处无法归约 E'",
            position=token.position,
            found=token.type,
            expected=["+", "-", ")", "EOF"],
        )

    def parse_T(self) -> Node:
        self.record("T", ["F", "T'"])
        f_node = self.parse_F()
        tp_node = self.parse_Tp()
        return Node("T", children=[f_node, tp_node])

    def parse_Tp(self) -> Node:
        token = self.current()
        if token.type == "MUL":
            self.record("T'", ["*", "F", "T'"])
            op = self.consume("MUL")
            f_node = self.parse_F()
            tp_node = self.parse_Tp()
            return Node("T'", children=[Node("op", value=op.value), f_node, tp_node])
        if token.type == "DIV":
            self.record("T'", ["/", "F", "T'"])
            op = self.consume("DIV")
            f_node = self.parse_F()
            tp_node = self.parse_Tp()
            return Node("T'", children=[Node("op", value=op.value), f_node, tp_node])
        if token.type in {"PLUS", "MINUS", "RPAREN", "EOF"}:
            self.record("T'", [])
            return Node("T'", value="ε")
        raise ParserError(
            message=f"位置 {token.position} 处无法归约 T'",
            position=token.position,
            found=token.type,
            expected=["*", "/", "+", "-", ")", "EOF"],
        )

    def parse_F(self) -> Node:
        token = self.current()
        if token.type == "INCREMENT":
            # 前缀 ++F
            self.record("F", ["++", "F"])
            op = self.consume("INCREMENT", "++")
            operand = self.parse_F()
            return Node("F", children=[Node("prefix", value=op.value), operand])
        if token.type == "DECREMENT":
            # 前缀 --F
            self.record("F", ["--", "F"])
            op = self.consume("DECREMENT", "--")
            operand = self.parse_F()
            return Node("F", children=[Node("prefix", value=op.value), operand])
        self.record("F", ["P", "S"])
        primary = self.parse_P()
        return self.parse_S(primary)

    def parse_P(self) -> Node:
        # 基本因子：括号、标识符或数字
        token = self.current()
        if token.type == "LPAREN":
            self.record("P", ["(", "E", ")"])
            self.consume("LPAREN", "(")
            e_node = self.parse_E()
            self.consume("RPAREN", ")")
            return Node("F", children=[Node("(", value="("), e_node, Node(")", value=")")])
        if token.type == "ID":
            self.record("P", ["id"])
            tok = self.consume("ID", "标识符")
            return Node("F", children=[Node("id", value=tok.value)])
        if token.type == "NUM":
            self.record("P", ["num"])
            tok = self.consume("NUM", "数字")
            return Node("F", children=[Node("num", value=tok.value)])
        raise ParserError(
            message=f"位置 {token.position} 处无法归约 F",
            position=token.position,
            found=token.type,
            expected=["(", "id", "num"],
        )

    def parse_S(self, base: Node) -> Node:
        token = self.current()
        if token.type == "INCREMENT":
            # 后缀 ++
            self.record("S", ["++", "S"])
            op = self.consume("INCREMENT", "++")
            combined = Node("F", children=[base, Node("postfix", value=op.value)])
            return self.parse_S(combined)
        if token.type == "DECREMENT":
            # 后缀 --
            self.record("S", ["--", "S"])
            op = self.consume("DECREMENT", "--")
            combined = Node("F", children=[base, Node("postfix", value=op.value)])
            return self.parse_S(combined)
        self.record("S", [])
        return base


def format_tokens(tokens: List[Token]) -> str:
    lines = []
    for t in tokens:
        if t.type == "EOF":
            lines.append(f"{len(lines)}: EOF (pos {t.position})")
        else:
            lines.append(f"{len(lines)}: {t.type}='{t.value}' (pos {t.position})")
    return "\n".join(lines)


def format_tree(node: Node) -> str:
    lines: List[str] = []

    def walk(current: Node, prefix: str = "", is_last: bool = True, is_root: bool = True):
        label = current.label if current.value is None else f"{current.label}: {current.value}"
        if is_root:
            lines.append(label)
        else:
            connector = "└── " if is_last else "├── "
            lines.append(prefix + connector + label)
        new_prefix = prefix + ("    " if is_last else "│   ")
        for idx, child in enumerate(current.children):
            walk(child, new_prefix, idx == len(current.children) - 1, False)

    walk(node)
    return "\n".join(lines)


def format_derivation(steps: List[str]) -> str:
    return "\n".join(f"{idx}. {step}" for idx, step in enumerate(steps, start=1))

ICON_PATH = Path(__file__).with_name("logo.ico")


def load_app_icon() -> QIcon:
    if ICON_PATH.exists():
        return QIcon(str(ICON_PATH))
    return QIcon()

class ParserWindow(QMainWindow):
    def __init__(self, app_icon: QIcon):
        # GUI 层：展示实时语法分析结果
        super().__init__()
        self.setWindowTitle("燕山大学编译原理课程设计")
        self.setWindowIcon(app_icon)
        self.resize(1080, 830)
        self.setMinimumSize(880, 600)
        self.delay_timer = QTimer(self)
        self.delay_timer.setSingleShot(True)
        self.delay_timer.timeout.connect(self.run_analysis)
        self._build_ui()

    def _build_ui(self):
        self.setStyleSheet(
            """
            QMainWindow { background-color: #eef1f6; }
            QLabel#TitleLabel { font-size: 26px; font-weight: 600; color: #122546; letter-spacing: 1px; }
            QLabel#SubtitleLabel { color: #556079; font-size: 14px; }
            QFrame#card {
                background: rgba(255, 255, 255, 0.92);
                border-radius: 16px;
                border: 1px solid #dce5f7;
            }
            QLabel#cardTitle { font-weight: 600; color: #14213d; }
            QLineEdit {
                border: 1px solid #b8c7e4;
                border-radius: 12px;
                padding: 10px 14px;
                font-size: 15px;
                background: rgba(255,255,255,0.95);
            }
            QLineEdit:focus { border-color: #4876d3; }
            QPushButton#GhostButton {
                border: 1px solid #c7d3ea;
                border-radius: 10px;
                padding: 6px 18px;
                background: rgba(255,255,255,0.9);
                color: #465169;
            }
            QPushButton#GhostButton:hover { border-color: #4876d3; color: #122546; }
            QPlainTextEdit {
                background: rgba(249, 250, 255, 0.9);
                border: 1px solid #dfe4f3;
                border-radius: 12px;
                font-family: "Fira Code", "Consolas", monospace;
                font-size: 13px;
                padding: 8px;
            }
            QLabel#StatusLabel {
                font-size: 18px;
                font-weight: 600;
            }
            """
        )

        central = QWidget()
        central.setAttribute(Qt.WA_StyledBackground, True)
        self.setCentralWidget(central)
        layout = QVBoxLayout()
        layout.setContentsMargins(28, 20, 28, 16)
        layout.setSpacing(14)
        central.setLayout(layout)

        title = QLabel("递归下降LL(1)语法分析")
        title.setObjectName("TitleLabel")
        subtitle = QLabel("实时递归下降推导 · Tokens / 判定 / 推导 · 语法树")
        subtitle.setObjectName("SubtitleLabel")
        layout.addWidget(title)
        layout.addWidget(subtitle)

        input_frame = QFrame()
        input_frame.setObjectName("card")
        input_frame.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Fixed)
        input_layout = QVBoxLayout(input_frame)
        input_layout.setContentsMargins(20, 18, 20, 16)
        input_layout.setSpacing(10)
        input_layout.addWidget(QLabel("输入表达式，系统将自动实时分析："))

        input_row = QHBoxLayout()
        input_row.setSpacing(12)
        self.input_edit = QLineEdit()
        self.input_edit.setPlaceholderText("例如：a+3*(b-2)")
        input_row.addWidget(self.input_edit)
        button_stack = QVBoxLayout()
        button_stack.setSpacing(8)
        self.batch_button = QPushButton("批量处理")
        self.batch_button.setObjectName("GhostButton")
        self.clear_button = QPushButton("清空")
        self.clear_button.setObjectName("GhostButton")
        button_stack.addWidget(self.batch_button)
        button_stack.addWidget(self.clear_button)
        button_stack.addStretch()
        input_row.addLayout(button_stack)
        input_layout.addLayout(input_row)
        layout.addWidget(input_frame)

        status_frame = QFrame()
        status_frame.setObjectName("card")
        status_frame.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Fixed)
        status_layout = QVBoxLayout(status_frame)
        status_layout.setContentsMargins(20, 12, 20, 12)
        self.result_label = QLabel("判定：等待输入")
        self.result_label.setObjectName("StatusLabel")
        self.status_hint = QLabel("在输入框内键入表达式即可实时看到结果")
        self.status_hint.setStyleSheet("color: #6c7692;")
        status_layout.addWidget(self.result_label)
        status_layout.addWidget(self.status_hint)
        layout.addWidget(status_frame)

        hint_frame, self.hint_box = self._create_card("提示信息", min_height=60, wrap=True)
        hint_frame.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Fixed)
        self.hint_box.setFixedHeight(60)
        self.hint_box.setMaximumBlockCount(3)
        self.hint_box.setWordWrapMode(QTextOption.WordWrap)
        layout.addWidget(hint_frame)

        tokens_frame, self.tokens_box = self._create_card("Tokens", min_height=0)
        derivation_frame, self.derivation_box = self._create_card("最左推导", min_height=0)
        tree_frame, self.tree_box = self._create_card("语法树", min_height=0)

        left_splitter = QSplitter(Qt.Vertical)
        left_splitter.setChildrenCollapsible(False)
        left_splitter.addWidget(tokens_frame)
        left_splitter.addWidget(derivation_frame)
        left_splitter.setSizes([1, 1])

        outputs_splitter = QSplitter(Qt.Horizontal)
        outputs_splitter.setChildrenCollapsible(False)
        outputs_splitter.addWidget(left_splitter)
        outputs_splitter.addWidget(tree_frame)
        outputs_splitter.setSizes([1, 1])
        layout.addWidget(outputs_splitter, 1)

        footer = QLabel("计科23级5班李济岑作品")
        footer.setAlignment(Qt.AlignCenter)
        footer.setStyleSheet("color: #6c7692; font-size: 13px;")
        footer.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Fixed)
        layout.addWidget(footer)

        self.batch_button.clicked.connect(self.run_batch_processing)
        self.clear_button.clicked.connect(self.clear_all)
        self.input_edit.textChanged.connect(self.schedule_analysis)

    def _create_card(self, title: str, min_height: int = 140, wrap: bool = False):
        frame = QFrame()
        frame.setObjectName("card")
        frame.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        frame_layout = QVBoxLayout(frame)
        frame_layout.setContentsMargins(20, 16, 20, 20)
        frame_layout.setSpacing(10)
        label = QLabel(title)
        label.setObjectName("cardTitle")
        box = QPlainTextEdit()
        box.setReadOnly(True)
        box.setMinimumHeight(min_height)
        box.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        box.setVerticalScrollBarPolicy(Qt.ScrollBarAsNeeded)
        box.setHorizontalScrollBarPolicy(Qt.ScrollBarAsNeeded)
        if wrap:
            box.setLineWrapMode(QPlainTextEdit.WidgetWidth)
        else:
            box.setLineWrapMode(QPlainTextEdit.NoWrap)
        frame_layout.addWidget(label)
        frame_layout.addWidget(box, 1)
        return frame, box

    def schedule_analysis(self):
        text = self.input_edit.text()
        if not text.strip():
            self.delay_timer.stop()
            self._show_status("判定：等待输入", mode="neutral", hint="输入框为空，等待内容……")
            self._clear_outputs()
            self.hint_box.clear()
            return
        self._show_status("判定：实时分析中……", mode="neutral", hint="输入暂停 0.25 秒后自动更新结果")
        self.delay_timer.start(250)

    def run_analysis(self):
        expr = self.input_edit.text().strip()
        if not expr:
            return
        try:
            tokens = tokenize(expr)
        except LexerError as err:
            self._show_status("判定：词法错误", mode="error", hint="请检查是否有非法字符")
            self._clear_outputs()
            self.hint_box.setPlainText(str(err))
            return

        parser = Parser(tokens)
        try:
            tree = parser.parse()
        except ParserError as err:
            expected = ", ".join(err.expected)
            self.tokens_box.setPlainText(format_tokens(tokens))
            self.derivation_box.setPlainText(format_derivation(parser.derivation_steps))
            self.tree_box.setPlainText("(本次失败，未生成语法树)")
            message = (
                f"错误位置：索引 {err.position}，遇到 {err.found}，期望 {expected}\\n提示：{err}"
            )
            self.hint_box.setPlainText(message)
            self._show_status("判定：不合法表达式", mode="error", hint="根据提示修复语法后继续输入")
            return

        self.tokens_box.setPlainText(format_tokens(tokens))
        self.derivation_box.setPlainText(format_derivation(parser.derivation_steps))
        self.tree_box.setPlainText(format_tree(tree))
        self.hint_box.setPlainText("解析成功，可结合推导与语法树讲解。")
        self._show_status("判定：合法表达式", mode="ok", hint="所有检查通过，语法树已更新")

    def clear_all(self):
        self.delay_timer.stop()
        self.input_edit.clear()
        self._clear_outputs()
        self.hint_box.clear()
        self._show_status("判定：等待输入", mode="neutral", hint="输入框已清空")

    def run_batch_processing(self):
        file_path, _ = QFileDialog.getOpenFileName(
            self, "选择 input.txt", "", "Text Files (*.txt);;All Files (*)"
        )
        if not file_path:
            return
        input_path = Path(file_path)
        try:
            lines = input_path.read_text(encoding="utf-8").splitlines()
        except OSError as err:
            self.hint_box.setPlainText(f"无法读取文件：{err}")
            self._show_status("判定：批量失败", mode="error", hint="请检查文件权限或路径")
            return
        results = []
        case_no = 1
        processed = 0
        for raw in lines:
            expr = raw.strip()
            if not expr:
                continue
            processed += 1
            info = self._process_expression(expr)
            block = [f"[用例 {case_no}] 原始：{expr}"]
            block.append(f"结果：{'合法' if info['success'] else '不合法'}")
            if info.get("error"):
                block.append(info["error"])
            if info.get("tokens"):
                block.append("Tokens:")
                block.append(info["tokens"])
            if info.get("derivation"):
                block.append("最左推导:")
                block.append(info["derivation"])
            if info.get("tree"):
                block.append("语法树:")
                block.append(info["tree"])
            results.append("\n".join(block))
            case_no += 1
        if not results:
            self.hint_box.setPlainText("文件中没有可解析的表达式。")
            self._show_status("判定：批量结束", mode="neutral", hint="空文件或仅含空行")
            return
        output_path = input_path.with_name("output.txt")
        separator = "\n" + ("-" * 50) + "\n"
        try:
            output_path.write_text(separator.join(results), encoding="utf-8")
        except OSError as err:
            self.hint_box.setPlainText(f"写出 output.txt 失败：{err}")
            self._show_status("判定：批量失败", mode="error", hint="请检查磁盘权限")
            return
        self.hint_box.setPlainText(
            f"批量处理完成，共 {processed} 条输入。\n输出文件：{output_path}"
        )
        self._show_status("判定：批量完成", mode="ok", hint="output.txt 已生成")
        QMessageBox.information(self, "批量处理", "已批量完成")

    def _process_expression(self, expr: str):
        info = {"expression": expr, "success": False}
        if not expr:
            info["error"] = "空表达式"
            return info
        try:
            tokens = tokenize(expr)
        except LexerError as err:
            info["error"] = f"词法错误：{err}"
            return info
        parser = Parser(tokens)
        try:
            tree = parser.parse()
        except ParserError as err:
            expected = ", ".join(err.expected)
            detail = (
                f"语法错误：索引 {err.position}，遇到 {err.found}，期望 {expected}"
            )
            info["error"] = detail
            info["tokens"] = format_tokens(tokens)
            info["derivation"] = format_derivation(parser.derivation_steps)
            return info
        info["success"] = True
        info["tokens"] = format_tokens(tokens)
        info["derivation"] = format_derivation(parser.derivation_steps)
        info["tree"] = format_tree(tree)
        return info

    def _clear_outputs(self):
        self.tokens_box.clear()
        self.derivation_box.clear()
        self.tree_box.clear()

    def _show_status(self, text: str, mode: str, hint: Optional[str] = None):
        colors = {
            "neutral": "#62708f",
            "ok": "#2a9d8f",
            "error": "#c44536",
        }
        self.result_label.setText(text)
        color = colors.get(mode, "#62708f")
        self.result_label.setStyleSheet(f"color: {color}; font-size: 20px; font-weight: 600;")
        if hint is not None:
            self.status_hint.setText(hint)

def main():
    app = QApplication(sys.argv)
    app.setApplicationName("燕山大学编译原理课程设计")
    icon = load_app_icon()
    if not icon.isNull():
        app.setWindowIcon(icon)
    window = ParserWindow(icon)
    window.show()
    sys.exit(app.exec_())


if __name__ == "__main__":
    main()
