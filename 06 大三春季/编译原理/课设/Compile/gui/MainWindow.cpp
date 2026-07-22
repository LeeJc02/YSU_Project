#include "MainWindow.hpp"
#include "CodeEditor.hpp"

#include <QAbstractItemView>
#include <QAction>
#include <QApplication>
#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QDockWidget>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontDatabase>
#include <QGraphicsOpacityEffect>
#include <QHeaderView>
#include <QIcon>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QPainter>
#include <QLinearGradient>
#include <QPlainTextEdit>
#include <QResizeEvent>
#include <QRegularExpression>
#include <QScrollArea>
#include <QSplitter>
#include <QStatusBar>
#include <QTabWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextStream>
#include <QToolBar>
#include <QVBoxLayout>
#include <QStringList>
#include <QStandardPaths>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <type_traits>

#include "pl0/AST.hpp"
#include "pl0/Driver.hpp"
#include "pl0/SymbolTable.hpp"
#include "pl0/Token.hpp"

namespace {

struct TreeNode {
  QString label;
  std::vector<TreeNode> children;
};

QString binaryOpName(pl0::BinaryOp op);
QString unaryOpName(pl0::UnaryOp op);
QString assignmentOpName(pl0::AssignmentOperator op);

TreeNode buildExpressionTree(const pl0::Expression& expr);
TreeNode buildStatementTree(const pl0::Statement& stmt);
TreeNode buildBlockTree(const pl0::Block& block);
TreeNode buildProgramTree(const pl0::Program& program) {
  TreeNode root;
  root.label = QObject::tr("Program");
  root.children.push_back(buildBlockTree(program.block));
  return root;
}

TreeNode buildExpressionTree(const pl0::Expression& expr) {
  TreeNode node;
  std::visit(
      [&](const auto& value) {
        using T = std::decay_t<decltype(value)>;
        if constexpr (std::is_same_v<T, pl0::NumberLiteral>) {
          node.label = QString::number(value.value);
        } else if constexpr (std::is_same_v<T, pl0::BooleanLiteral>) {
          node.label = value.value ? QObject::tr("true") : QObject::tr("false");
        } else if constexpr (std::is_same_v<T, pl0::IdentifierExpr>) {
          node.label = QString::fromStdString(value.name);
        } else if constexpr (std::is_same_v<T, pl0::ArrayAccessExpr>) {
          node.label = QObject::tr("数组访问: %1").arg(QString::fromStdString(value.name));
          node.children.push_back(buildExpressionTree(*value.index));
        } else if constexpr (std::is_same_v<T, pl0::BinaryExpr>) {
          node.label = binaryOpName(value.op);
          node.children.push_back(buildExpressionTree(*value.lhs));
          node.children.push_back(buildExpressionTree(*value.rhs));
        } else if constexpr (std::is_same_v<T, pl0::UnaryExpr>) {
          node.label = unaryOpName(value.op);
          node.children.push_back(buildExpressionTree(*value.operand));
        } else if constexpr (std::is_same_v<T, pl0::CallExpr>) {
          node.label = QObject::tr("函数调用: %1")
                          .arg(QString::fromStdString(value.callee));
          for (const auto& arg : value.arguments) {
            node.children.push_back(buildExpressionTree(*arg));
          }
        }
      },
      expr.value);
  return node;
}

QString assignmentOpName(pl0::AssignmentOperator op) {
  switch (op) {
    case pl0::AssignmentOperator::Assign:
      return QObject::tr(":=");
    case pl0::AssignmentOperator::AddAssign:
      return QObject::tr("+=");
    case pl0::AssignmentOperator::SubAssign:
      return QObject::tr("-=");
    case pl0::AssignmentOperator::MulAssign:
      return QObject::tr("*=");
    case pl0::AssignmentOperator::DivAssign:
      return QObject::tr("/=");
    case pl0::AssignmentOperator::ModAssign:
      return QObject::tr("%=");
  }
  return QObject::tr(":=");
}

TreeNode buildStatementTree(const pl0::Statement& stmt) {
  return std::visit(
      [&](const auto& value) -> TreeNode {
        using T = std::decay_t<decltype(value)>;
        TreeNode node;
        if constexpr (std::is_same_v<T, pl0::AssignmentStmt>) {
          node.label = QObject::tr("赋值(%1): %2")
                           .arg(assignmentOpName(value.op),
                                QString::fromStdString(value.target));
          if (value.index) {
            TreeNode indexNode;
            indexNode.label = QObject::tr("索引");
            indexNode.children.push_back(buildExpressionTree(*value.index));
            node.children.push_back(std::move(indexNode));
          }
          node.children.push_back(buildExpressionTree(*value.value));
        } else if constexpr (std::is_same_v<T, pl0::CallStmt>) {
          node.label = QObject::tr("调用: %1")
                           .arg(QString::fromStdString(value.callee));
          for (const auto& arg : value.arguments) {
            node.children.push_back(buildExpressionTree(*arg));
          }
        } else if constexpr (std::is_same_v<T, pl0::IfStmt>) {
          node.label = QObject::tr("条件语句");
          TreeNode cond;
          cond.label = QObject::tr("条件");
          cond.children.push_back(buildExpressionTree(*value.condition));
          node.children.push_back(std::move(cond));
          TreeNode thenNode;
          thenNode.label = QObject::tr("Then");
          for (const auto& s : value.then_branch) {
            thenNode.children.push_back(buildStatementTree(*s));
          }
          node.children.push_back(std::move(thenNode));
          if (!value.else_branch.empty()) {
            TreeNode elseNode;
            elseNode.label = QObject::tr("Else");
            for (const auto& s : value.else_branch) {
              elseNode.children.push_back(buildStatementTree(*s));
            }
            node.children.push_back(std::move(elseNode));
          }
        } else if constexpr (std::is_same_v<T, pl0::WhileStmt>) {
          node.label = QObject::tr("当型循环");
          TreeNode cond;
          cond.label = QObject::tr("条件");
          cond.children.push_back(buildExpressionTree(*value.condition));
          node.children.push_back(std::move(cond));
          TreeNode body;
          body.label = QObject::tr("循环体");
          for (const auto& s : value.body) {
            body.children.push_back(buildStatementTree(*s));
          }
          node.children.push_back(std::move(body));
        } else if constexpr (std::is_same_v<T, pl0::RepeatStmt>) {
          node.label = QObject::tr("重复循环");
          TreeNode body;
          body.label = QObject::tr("循环体");
          for (const auto& s : value.body) {
            body.children.push_back(buildStatementTree(*s));
          }
          node.children.push_back(std::move(body));
          TreeNode until;
          until.label = QObject::tr("直到");
          until.children.push_back(buildExpressionTree(*value.condition));
          node.children.push_back(std::move(until));
        } else if constexpr (std::is_same_v<T, pl0::ReadStmt>) {
          node.label = QObject::tr("读入");
          for (const auto& target : value.targets) {
            TreeNode child;
            child.label = QString::fromStdString(target);
            node.children.push_back(std::move(child));
          }
        } else if constexpr (std::is_same_v<T, pl0::WriteStmt>) {
          node.label = value.newline ? QObject::tr("输出并换行")
                                     : QObject::tr("输出");
          for (const auto& exprPtr : value.values) {
            node.children.push_back(buildExpressionTree(*exprPtr));
          }
          if (value.newline) {
            TreeNode newlineNode;
            newlineNode.label = QObject::tr("换行");
            node.children.push_back(std::move(newlineNode));
          }
        } else if constexpr (std::is_same_v<T, std::vector<pl0::StmtPtr>>) {
          node.label = QObject::tr("复合语句");
          for (const auto& stmtPtr : value) {
            node.children.push_back(buildStatementTree(*stmtPtr));
          }
        }
        return node;
      },
      stmt.value);
}

TreeNode buildBlockTree(const pl0::Block& block) {
  TreeNode node;
  node.label = QObject::tr("Block");
  if (!block.consts.empty()) {
    TreeNode constNode;
    constNode.label = QObject::tr("常量");
    for (const auto& c : block.consts) {
      TreeNode child;
      child.label = QString::fromStdString(c.name + " = " + std::to_string(c.value));
      constNode.children.push_back(std::move(child));
    }
    node.children.push_back(std::move(constNode));
  }
  if (!block.vars.empty()) {
    TreeNode varNode;
    varNode.label = QObject::tr("变量");
    for (const auto& v : block.vars) {
      QString label = QString::fromStdString(v.name);
      if (v.array_size) {
        label += QStringLiteral("[%1]").arg(static_cast<unsigned>(*v.array_size));
      }
      TreeNode child;
      child.label = label;
      varNode.children.push_back(std::move(child));
    }
    node.children.push_back(std::move(varNode));
  }
  if (!block.procedures.empty()) {
    TreeNode procNode;
    procNode.label = QObject::tr("过程");
    for (const auto& proc : block.procedures) {
      TreeNode child;
      child.label = QObject::tr("过程: %1").arg(QString::fromStdString(proc.name));
      if (proc.body) {
        child.children.push_back(buildBlockTree(*proc.body));
      }
      procNode.children.push_back(std::move(child));
    }
    node.children.push_back(std::move(procNode));
  }
  if (!block.statements.empty()) {
    TreeNode stmts;
    stmts.label = QObject::tr("语句");
    for (const auto& stmt : block.statements) {
      stmts.children.push_back(buildStatementTree(*stmt));
    }
    node.children.push_back(std::move(stmts));
  }
  return node;
}

struct LayoutNode {
  TreeNode data;
  std::vector<LayoutNode> children;
  double subtreeWidth = 0.0;
  QRectF rect;
};

double prepareLayout(LayoutNode& layout, const TreeNode& node,
                     const QFontMetrics& metrics, double hSpacing,
                     double vSpacing) {
  layout.data = node;
  layout.children.resize(node.children.size());
  const double paddingX = 28.0;
  const double paddingY = 20.0;
  const QSize textSize = metrics.size(Qt::TextSingleLine, node.label);
  layout.rect.setSize(QSizeF(textSize.width() + paddingX,
                             textSize.height() + paddingY));

  if (layout.children.empty()) {
    layout.subtreeWidth = layout.rect.width();
    return layout.subtreeWidth;
  }

  double totalChildrenWidth = 0.0;
  for (std::size_t i = 0; i < node.children.size(); ++i) {
    totalChildrenWidth += prepareLayout(layout.children[i], node.children[i],
                                        metrics, hSpacing, vSpacing);
  }
  if (!layout.children.empty()) {
    totalChildrenWidth += hSpacing * (layout.children.size() - 1);
  }
  layout.subtreeWidth = std::max(layout.rect.width(), totalChildrenWidth);
  return layout.subtreeWidth;
}

double assignPositions(LayoutNode& layout, double left, double top,
                       double hSpacing, double vSpacing) {
  layout.rect.moveTo(left + (layout.subtreeWidth - layout.rect.width()) / 2.0,
                     top);
  double bottom = layout.rect.bottom();

  if (layout.children.empty()) {
    return bottom;
  }

  double combinedWidth = 0.0;
  for (const auto& child : layout.children) {
    combinedWidth += child.subtreeWidth;
  }
  combinedWidth += hSpacing * (layout.children.size() - 1);

  double currentLeft = left + (layout.subtreeWidth - combinedWidth) / 2.0;
  const double childTop = layout.rect.bottom() + vSpacing;
  for (auto& child : layout.children) {
    bottom = std::max(
        bottom, assignPositions(child, currentLeft, childTop, hSpacing, vSpacing));
    currentLeft += child.subtreeWidth + hSpacing;
  }
  return bottom;
}

void drawLayout(QPainter& painter, const LayoutNode& layout, const QFont& font) {
  painter.setPen(QPen(QColor(153, 169, 205), 1.2, Qt::SolidLine, Qt::RoundCap));
  painter.setBrush(QColor(244, 247, 255, 235));
  QRectF rect = layout.rect;
  painter.drawRoundedRect(rect, 10, 10);
  painter.setPen(QColor(40, 53, 85));
  painter.setFont(font);
  painter.drawText(rect, Qt::AlignCenter, layout.data.label);

  painter.setPen(QPen(QColor(185, 196, 221), 1.0, Qt::SolidLine, Qt::RoundCap));
  for (const auto& child : layout.children) {
    QPointF from(rect.center().x(), rect.bottom());
    QPointF to(child.rect.center().x(), child.rect.top());
    painter.drawLine(from, to);
    drawLayout(painter, child, font);
  }
}

QString diagnosticLevelToString(pl0::DiagnosticLevel level) {
  switch (level) {
    case pl0::DiagnosticLevel::Error:
      return QObject::tr("错误");
    case pl0::DiagnosticLevel::Warning:
      return QObject::tr("警告");
    case pl0::DiagnosticLevel::Note:
      return QObject::tr("提示");
  }
  return QObject::tr("未知");
}

QString sourceRangeToString(const pl0::SourceRange& range) {
  return QString::asprintf("%zu:%zu-%zu:%zu", range.begin.line,
                           range.begin.column, range.end.line,
                           range.end.column);
}

QString binaryOpName(pl0::BinaryOp op) {
  using pl0::BinaryOp;
  switch (op) {
    case BinaryOp::Add:
      return QObject::tr("加法");
    case BinaryOp::Subtract:
      return QObject::tr("减法");
    case BinaryOp::Multiply:
      return QObject::tr("乘法");
    case BinaryOp::Divide:
      return QObject::tr("除法");
    case BinaryOp::Modulo:
      return QObject::tr("取模");
    case BinaryOp::Equal:
      return QObject::tr("等于");
    case BinaryOp::NotEqual:
      return QObject::tr("不等于");
    case BinaryOp::Less:
      return QObject::tr("小于");
    case BinaryOp::LessEqual:
      return QObject::tr("小于等于");
    case BinaryOp::Greater:
      return QObject::tr("大于");
    case BinaryOp::GreaterEqual:
      return QObject::tr("大于等于");
    case BinaryOp::And:
      return QObject::tr("逻辑与");
    case BinaryOp::Or:
      return QObject::tr("逻辑或");
  }
  return QObject::tr("未知二元操作");
}

QString unaryOpName(pl0::UnaryOp op) {
  using pl0::UnaryOp;
  switch (op) {
    case UnaryOp::Positive:
      return QObject::tr("正号");
    case UnaryOp::Negative:
      return QObject::tr("负号");
    case UnaryOp::Not:
      return QObject::tr("逻辑非");
    case UnaryOp::Odd:
      return QObject::tr("奇偶判断");
  }
  return QObject::tr("未知一元操作");
}

class StreamRedirector {
 public:
  StreamRedirector(std::ios& stream, std::streambuf* new_buffer)
      : stream_(stream), old_buffer_(stream.rdbuf(new_buffer)) {}

  ~StreamRedirector() { stream_.rdbuf(old_buffer_); }

 private:
  std::ios& stream_;
  std::streambuf* old_buffer_ = nullptr;
};

}  // namespace

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
  setupUi();
  setupMenus();
  setupConnections();
  updateWindowTitle();
}

void MainWindow::setupUi() {
  resize(1100, 720);
  setWindowTitle(tr("编译原理展示"));

  originalUiFont_ = font();

  baseMonospaceFont_ = QFontDatabase::systemFont(QFontDatabase::FixedFont);
  baseMonospaceFont_.setPointSize(baseMonospaceFont_.pointSize() + 5);
  baseUiFont_ = font();
  baseUiFont_.setPointSize(baseUiFont_.pointSize() + 6);
  setFont(baseUiFont_);
  setStyleSheet(QStringLiteral(
      "QMainWindow { background-color: #ffffff; color: #2f2f2f; }\n"
      "QPlainTextEdit { background-color: rgba(255,255,255,0.92); color: #1d1d1d; border: 1px solid #d8dbe8; border-radius: 6px; padding: 8px; }\n"
      "QTableWidget { background-color: rgba(255,255,255,0.9); color: #1d1d1d; gridline-color: #e5e7f2; selection-background-color: #d2e1ff; selection-color: #142952; }\n"
      "QTreeWidget { background-color: rgba(255,255,255,0.9); color: #1d1d1d; border: 1px solid #d8dbe8; border-radius: 6px; }\n"
      "QHeaderView::section { background-color: #f5f7ff; color: #20243a; border: 1px solid #e2e4f0; padding: 6px; font-weight: 600; }\n"
      "QStatusBar { background-color: rgba(245,245,249,0.88); color: #2f2f2f; border-top: 1px solid #e6e7ef; }\n"
      "QToolBar { background-color: rgba(247,248,253,0.92); border: 1px solid #e6e7ef; }\n"
      "QTabWidget::pane { border: 1px solid #dadced; background-color: rgba(255,255,255,0.86); border-radius: 6px; }\n"
      "QTabBar::tab { background-color: #f7f8fd; color: #38405f; padding: 6px 16px; margin: 3px; border: 1px solid #dfe2f2; border-radius: 6px; }\n"
      "QTabBar::tab:selected { background-color: #e7f0ff; color: #153b7a; border: 1px solid #adc6ff; }\n"
      "QDockWidget { titlebar-close-icon: url(); titlebar-normal-icon: url(); }\n"));

  initialWindowSize_ = size();

  backgroundLabel_ = new QLabel(this);
  backgroundLabel_->setAttribute(Qt::WA_TransparentForMouseEvents);
  backgroundLabel_->setScaledContents(true);
  backgroundOpacityEffect_ = new QGraphicsOpacityEffect(backgroundLabel_);
  backgroundOpacityEffect_->setOpacity(0.7);
  backgroundLabel_->setGraphicsEffect(backgroundOpacityEffect_);

  const QStringList candidates = {
      QStringLiteral(":/ysu.jpg"),
      QCoreApplication::applicationDirPath() + QStringLiteral("/ysu.jpg"),
      QStringLiteral("ysu.jpg"),
      QStringLiteral(":/ysu.jpg"),
      QCoreApplication::applicationDirPath() + QStringLiteral("/ysu.jpg"),
      QStringLiteral("ysu.jpg")};
  for (const QString& path : candidates) {
    QPixmap pix(path);
    if (!pix.isNull()) {
      backgroundPixmap_ = pix;
      break;
    }
  }
  if (backgroundPixmap_.isNull()) {
    backgroundPixmap_ = QPixmap(200, 200);
    backgroundPixmap_.fill(Qt::transparent);
    QPainter painter(&backgroundPixmap_);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    QLinearGradient grad(0, 0, 200, 200);
    grad.setColorAt(0.0, QColor(230, 235, 255));
    grad.setColorAt(1.0, QColor(190, 205, 255));
    painter.setBrush(grad);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(backgroundPixmap_.rect(), 40, 40);
    painter.setPen(QPen(QColor(80, 95, 150), 2));
    painter.drawRoundedRect(backgroundPixmap_.rect().adjusted(4, 4, -4, -4), 36, 36);
    QFont logoFont = baseUiFont_;
    logoFont.setPointSize(28);
    logoFont.setBold(true);
    painter.setFont(logoFont);
    painter.setPen(QColor(40, 55, 110));
    painter.drawText(backgroundPixmap_.rect(), Qt::AlignCenter, tr("PL/0"));
  }
  if (!backgroundPixmap_.isNull()) {
    setWindowIcon(QIcon(backgroundPixmap_));
  }

  auto* splitter = new QSplitter(Qt::Horizontal, this);
  sourceEdit_ = new CodeEditor(splitter);
  sourceEdit_->setPlaceholderText(tr("在此编写 PL/0 源代码..."));
  sourceEdit_->setTabStopDistance(4 * sourceEdit_->fontMetrics().horizontalAdvance(' '));
  sourceEdit_->setFont(baseMonospaceFont_);
  sourceEdit_->setLineNumberFont(baseMonospaceFont_);

  rightTabs_ = new QTabWidget(splitter);
  rightTabs_->setElideMode(Qt::ElideRight);

  tokensTable_ = new QTableWidget(rightTabs_);
  tokensTable_->setColumnCount(5);
  tokensTable_->setHorizontalHeaderLabels(
      {tr("索引"), tr("类型"), tr("词素"), tr("范围"), tr("值")});
  tokensTable_->horizontalHeader()->setStretchLastSection(true);
  tokensTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  tokensTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
  tokensTable_->setAlternatingRowColors(true);
  tokensTable_->setStyleSheet(tokensTable_->styleSheet() +
                              "QTableWidget { alternate-background-color: #f2f5ff; }");
  rightTabs_->addTab(tokensTable_, tr("词法单元"));

  astImageScroll_ = new QScrollArea(rightTabs_);
  astImageScroll_->setWidgetResizable(true);
  astImageScroll_->setFrameShape(QFrame::NoFrame);
  astImageLabel_ = new QLabel(astImageScroll_);
  astImageLabel_->setAlignment(Qt::AlignCenter);
  astImageLabel_->setBackgroundRole(QPalette::Base);
  astImageLabel_->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  astImageScroll_->setWidget(astImageLabel_);
  rightTabs_->addTab(astImageScroll_, tr("语法树图"));

  symbolsTable_ = new QTableWidget(rightTabs_);
  symbolsTable_->setColumnCount(7);
  symbolsTable_->setHorizontalHeaderLabels(
      {tr("名称"), tr("种类"), tr("类型"), tr("层次"), tr("地址"),
       tr("大小"), tr("传值")});
  symbolsTable_->horizontalHeader()->setStretchLastSection(true);
  symbolsTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  symbolsTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
  symbolsTable_->setAlternatingRowColors(true);
  symbolsTable_->setStyleSheet(symbolsTable_->styleSheet() +
                               "QTableWidget { alternate-background-color: #f2f5ff; }");
  rightTabs_->addTab(symbolsTable_, tr("符号表"));

  pcodeEdit_ = new CodeEditor(rightTabs_);
  pcodeEdit_->setReadOnly(true);
  pcodeEdit_->setFont(baseMonospaceFont_);
  pcodeEdit_->setLineNumberFont(baseMonospaceFont_);
  rightTabs_->addTab(pcodeEdit_, tr("P-Code"));

  diagnosticsEdit_ = new CodeEditor(rightTabs_);
  diagnosticsEdit_->setReadOnly(true);
  diagnosticsEdit_->setFont(baseMonospaceFont_);
  diagnosticsEdit_->setLineNumberFont(baseMonospaceFont_);
  rightTabs_->addTab(diagnosticsEdit_, tr("诊断信息"));

  vmOutputEdit_ = new CodeEditor(rightTabs_);
  vmOutputEdit_->setReadOnly(true);
  vmOutputEdit_->setFont(baseMonospaceFont_);
  vmOutputEdit_->setLineNumberFont(baseMonospaceFont_);
  rightTabs_->addTab(vmOutputEdit_, tr("运行输出"));

  splitter->setStretchFactor(0, 1);
  splitter->setStretchFactor(1, 2);
  setCentralWidget(splitter);

  auto* inputDock = new QDockWidget(tr("标准输入"), this);
  stdinEdit_ = new CodeEditor(inputDock);
  stdinEdit_->setPlaceholderText(tr("运行时输入（以空格或换行分隔整数）"));
  stdinEdit_->setFont(baseMonospaceFont_);
  stdinEdit_->setLineNumberFont(baseMonospaceFont_);
  inputDock->setWidget(stdinEdit_);
  addDockWidget(Qt::BottomDockWidgetArea, inputDock);

  watermarkLabel_ = new QLabel(tr("燕山大学 李济岑作品"), this);
  watermarkLabel_->setAttribute(Qt::WA_TransparentForMouseEvents);
  watermarkLabel_->setStyleSheet(
      "color: rgba(64, 64, 64, 0.32); font-size: 22px; font-weight: 600;"
      "background: transparent;");
  watermarkLabel_->adjustSize();
  watermarkLabel_->raise();
  watermarkLabel_->move(width() - watermarkLabel_->width() - 24,
                        height() - watermarkLabel_->height() - 24);

  backgroundLabel_->lower();
  if (centralWidget()) centralWidget()->raise();

  QSize logoSize = backgroundPixmap_.isNull() ? QSize(140, 140)
                                              : backgroundPixmap_.size().scaled(
                                                    QSize(140, 140), Qt::KeepAspectRatio);
  backgroundLabel_->setFixedSize(logoSize);
  backgroundLabel_->setPixmap(backgroundPixmap_.scaled(backgroundLabel_->size(), Qt::KeepAspectRatio,
                                                       Qt::SmoothTransformation));

  statusBar()->showMessage(tr("准备就绪"));
  updateFonts();
  relayoutOverlays();
}

void MainWindow::setupMenus() {
  fileMenu_ = menuBar()->addMenu(tr("文件"));
  openAction_ = fileMenu_->addAction(tr("打开..."));
  openAction_->setShortcut(QKeySequence::Open);
  saveAction_ = fileMenu_->addAction(tr("保存"));
  saveAction_->setShortcut(QKeySequence::Save);
  saveAsAction_ = fileMenu_->addAction(tr("另存为..."));
  saveAsAction_->setShortcut(QKeySequence::SaveAs);
  fileMenu_->addSeparator();
  exitAction_ = fileMenu_->addAction(tr("退出"));

  buildMenu_ = menuBar()->addMenu(tr("构建"));
  compileAction_ = buildMenu_->addAction(tr("编译"));
  compileAction_->setShortcut(Qt::CTRL | Qt::Key_B);
  runAction_ = buildMenu_->addAction(tr("运行"));
  runAction_->setShortcut(Qt::CTRL | Qt::Key_R);
  compileRunAction_ = buildMenu_->addAction(tr("编译并运行"));
  compileRunAction_->setShortcut(Qt::CTRL | Qt::Key_E);

  optionsMenu_ = menuBar()->addMenu(tr("选项"));
  boundsCheckAction_ = optionsMenu_->addAction(tr("启用数组越界检查"));
  boundsCheckAction_->setCheckable(true);
  boundsCheckAction_->setChecked(true);
  traceVmAction_ = optionsMenu_->addAction(tr("跟踪虚拟机指令"));
  traceVmAction_->setCheckable(true);

  mainToolBar_ = addToolBar(tr("工具"));
  mainToolBar_->addAction(openAction_);
  mainToolBar_->addAction(saveAction_);
  mainToolBar_->addSeparator();
  mainToolBar_->addAction(compileAction_);
  mainToolBar_->addAction(runAction_);
  mainToolBar_->addAction(compileRunAction_);
  mainToolBar_->addSeparator();
  mainToolBar_->addAction(boundsCheckAction_);
  mainToolBar_->addAction(traceVmAction_);

  toolbarWidgets_.clear();
  for (QAction* action :
       {openAction_, saveAction_, compileAction_, runAction_, compileRunAction_,
        boundsCheckAction_, traceVmAction_}) {
    if (QWidget* w = mainToolBar_->widgetForAction(action)) {
      toolbarWidgets_.append(w);
    }
  }

  connect(openAction_, &QAction::triggered, this, &MainWindow::openFile);
  connect(saveAction_, &QAction::triggered, this, &MainWindow::saveFile);
  connect(saveAsAction_, &QAction::triggered, this, &MainWindow::saveFileAs);
  connect(exitAction_, &QAction::triggered, this, &QWidget::close);
  connect(compileAction_, &QAction::triggered, this, &MainWindow::compileSource);
  connect(runAction_, &QAction::triggered, this, &MainWindow::runProgram);
  connect(compileRunAction_, &QAction::triggered, this, &MainWindow::compileAndRun);
  updateFonts();
}

void MainWindow::setupConnections() {
  connect(sourceEdit_, &QPlainTextEdit::textChanged, this,
          &MainWindow::markDocumentDirty);
}

void MainWindow::updateFonts() {
  if (initialWindowSize_.isEmpty()) {
    initialWindowSize_ = size();
  }
  const double scale = std::clamp(static_cast<double>(width()) /
                                      static_cast<double>(initialWindowSize_.width()),
                                  0.85, 1.5);

  QFont mono = baseMonospaceFont_;
  mono.setPointSizeF(std::max(18.0, baseMonospaceFont_.pointSizeF() * scale));
  sourceEdit_->setFont(mono);
  sourceEdit_->setLineNumberFont(mono);
  pcodeEdit_->setFont(mono);
  pcodeEdit_->setLineNumberFont(mono);
  diagnosticsEdit_->setFont(mono);
  diagnosticsEdit_->setLineNumberFont(mono);
  vmOutputEdit_->setFont(mono);
  vmOutputEdit_->setLineNumberFont(mono);
  stdinEdit_->setFont(mono);
  stdinEdit_->setLineNumberFont(mono);

  QFont ui = baseUiFont_;
  ui.setPointSizeF(std::max(20.0, baseUiFont_.pointSizeF() * scale));
  rightTabs_->setFont(ui);
  QFont tableFont = ui;
  tableFont.setPointSizeF(ui.pointSizeF() - 1.0);
  tokensTable_->setFont(tableFont);
  tokensTable_->horizontalHeader()->setFont(ui);
  symbolsTable_->setFont(tableFont);
  symbolsTable_->horizontalHeader()->setFont(ui);
  if (astImageLabel_) {
    astImageLabel_->setFont(ui);
  }

  QFont menuFont = originalUiFont_;
  menuFont.setPointSize(originalUiFont_.pointSize() + 2);
  if (menuBar()) {
    menuBar()->setFont(menuFont);
  }
  for (QMenu* menu : {fileMenu_, buildMenu_, optionsMenu_}) {
    if (menu) {
      menu->setFont(menuFont);
    }
  }
  if (mainToolBar_) {
    mainToolBar_->setFont(menuFont);
  }
  for (QWidget* widget : toolbarWidgets_) {
    if (widget) {
      widget->setFont(menuFont);
    }
  }
  for (QAction* action : {openAction_, saveAction_, saveAsAction_, exitAction_,
                          compileAction_, runAction_, compileRunAction_,
                          boundsCheckAction_, traceVmAction_}) {
    if (action) {
      action->setFont(menuFont);
    }
  }

  if (watermarkLabel_) {
    QFont watermarkFont = ui;
    watermarkFont.setPointSizeF(ui.pointSizeF() + 6.0);
    watermarkLabel_->setFont(watermarkFont);
    watermarkLabel_->adjustSize();
  }

  if (lastResult_ && lastResult_->program) {
    updateAstDiagram(*lastResult_->program);
  }

  relayoutOverlays();
}

void MainWindow::relayoutOverlays() {
  const int topOffset = menuBar()->height() + (mainToolBar_ ? mainToolBar_->height() : 0);
  const int bottomOffset = statusBar()->height();

  if (centralWidget()) {
    centralWidget()->raise();
  }
  if (mainToolBar_) {
    mainToolBar_->raise();
  }

  if (backgroundLabel_) {
    QSize logoSize = backgroundLabel_->size();
    const int marginX = 48;
    const int marginY = 14;
    int watermarkHeight = watermarkLabel_ ? watermarkLabel_->height() : 0;
    int x = width() - logoSize.width() - marginX;
    int y = height() - logoSize.height() - bottomOffset - watermarkHeight - marginY;
    if (y < topOffset + marginY) {
      y = topOffset + marginY;
    }
    backgroundLabel_->move(x, y);
    backgroundLabel_->raise();
  }

  if (watermarkLabel_) {
    watermarkLabel_->adjustSize();
    watermarkLabel_->move(width() - watermarkLabel_->width() - 24,
                          height() - watermarkLabel_->height() - bottomOffset - 12);
    watermarkLabel_->raise();
  }
}

void MainWindow::updateAstDiagram(const pl0::Program& program) {
  if (!astImageLabel_) {
    return;
  }
  QFont diagramFont = baseUiFont_;
  diagramFont.setPointSizeF(std::max(12.0, baseUiFont_.pointSizeF() *
                                             std::clamp(static_cast<double>(width()) /
                                                            static_cast<double>(initialWindowSize_.width()),
                                                        0.85, 1.5)));
  QPixmap pixmap = createAstPixmap(program, diagramFont);
  if (!pixmap.isNull()) {
    astImageLabel_->setPixmap(pixmap);
    astImageLabel_->setMinimumSize(pixmap.size());
    exportAstImage(pixmap);
  } else {
    astImageLabel_->clear();
    astImageLabel_->setMinimumSize(QSize(0, 0));
  }
}

QPixmap MainWindow::createAstPixmap(const pl0::Program& program,
                                    const QFont& font) const {
  TreeNode root = buildProgramTree(program);
  LayoutNode layout;
  const double hSpacing = 48.0;
  const double vSpacing = 90.0;
  const double margin = 48.0;
  QFontMetrics metrics(font);
  prepareLayout(layout, root, metrics, hSpacing, vSpacing);
  double bottom = assignPositions(layout, 0.0, 0.0, hSpacing, vSpacing);
  const QSizeF imageSize(layout.subtreeWidth + margin * 2,
                         bottom + metrics.height() + margin * 2);
  QPixmap pixmap(imageSize.toSize());
  pixmap.fill(Qt::transparent);

  QPainter painter(&pixmap);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
  painter.translate(margin, margin);
  drawLayout(painter, layout, font);
  return pixmap;
}

void MainWindow::exportAstImage(const QPixmap& pixmap) const {
  if (pixmap.isNull()) {
    return;
  }
  QString outputPath;
  if (!currentFilePath_.isEmpty()) {
    QFileInfo info(currentFilePath_);
    if (info.absolutePath().isEmpty()) {
      return;
    }
    outputPath =
        info.absolutePath() + QLatin1Char('/') + info.completeBaseName() + QStringLiteral(".png");
  } else {
    QString basePath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    if (basePath.isEmpty()) {
      basePath = QDir::homePath();
    }
    QDir exportDir(basePath);
    if (!exportDir.exists(QStringLiteral("PL0_AST"))) {
      exportDir.mkpath(QStringLiteral("PL0_AST"));
    }
    const QString fallbackName =
        QStringLiteral("ast_%1").arg(QDateTime::currentDateTime().toString(QStringLiteral("yyyyMMdd_HHmmss")));
    outputPath = exportDir.filePath(QStringLiteral("PL0_AST/%1.png").arg(fallbackName));
  }
  if (pixmap.save(outputPath, "PNG")) {
    if (auto* status = const_cast<MainWindow*>(this)->statusBar()) {
      status->showMessage(tr("已导出语法树图: %1").arg(QDir::toNativeSeparators(outputPath)), 5000);
    }
    qInfo() << "AST exported to" << QDir::toNativeSeparators(outputPath);
  } else {
    qWarning() << "Failed to export AST image to" << outputPath;
  }
}

void MainWindow::resizeEvent(QResizeEvent* event) {
  QMainWindow::resizeEvent(event);
  updateFonts();
  relayoutOverlays();
}

void MainWindow::openFile() {
  if (!promptToSave()) {
    return;
  }
  const QString path = QFileDialog::getOpenFileName(
      this, tr("打开 PL/0 源文件"), QString(), tr("PL/0 源文件 (*.pl0);;所有文件 (*.*)"));
  if (path.isEmpty()) {
    return;
  }
  QFile file(path);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QMessageBox::warning(this, tr("无法打开文件"), file.errorString());
    return;
  }
  QTextStream in(&file);
  sourceEdit_->setPlainText(in.readAll());
  currentFilePath_ = path;
  documentDirty_ = false;
  lastResult_.reset();
  statusBar()->showMessage(tr("已打开 %1").arg(path), 4000);
  updateWindowTitle();
}

void MainWindow::saveFile() {
  if (currentFilePath_.isEmpty()) {
    saveFileAs();
    return;
  }
  if (saveToPath(currentFilePath_)) {
    statusBar()->showMessage(tr("已保存"), 3000);
  }
}

void MainWindow::saveFileAs() {
  const QString path = QFileDialog::getSaveFileName(
      this, tr("另存为"), currentFilePath_,
      tr("PL/0 源文件 (*.pl0);;所有文件 (*.*)"));
  if (path.isEmpty()) {
    return;
  }
  if (saveToPath(path)) {
    currentFilePath_ = path;
    statusBar()->showMessage(tr("已保存到 %1").arg(path), 3000);
  }
}

bool MainWindow::saveToPath(const QString& path) {
  QFile file(path);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QMessageBox::warning(this, tr("无法保存文件"), file.errorString());
    return false;
  }
  QTextStream out(&file);
  out << sourceEdit_->toPlainText();
  documentDirty_ = false;
  updateWindowTitle();
  return true;
}

bool MainWindow::promptToSave() {
  if (!documentDirty_) {
    return true;
  }
  const auto choice = QMessageBox::question(
      this, tr("保存更改"), tr("是否保存对当前文档的修改？"),
      QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  if (choice == QMessageBox::Cancel) {
    return false;
  }
  if (choice == QMessageBox::Yes) {
    saveFile();
  }
  return true;
}

void MainWindow::compileSource() {
  compileInternal(false);
}

void MainWindow::compileAndRun() {
  if (compileInternal(false)) {
    executeCompiledProgram();
  }
}

void MainWindow::runProgram() {
  if (!lastResult_ || !lastResult_->code.size()) {
    if (!compileInternal(false)) {
      return;
    }
  }
  executeCompiledProgram();
}

bool MainWindow::compileInternal(bool quiet) {
  pl0::CompilerOptions options;
  options.enable_bounds_check = boundsCheckAction_->isChecked();

  pl0::DiagnosticSink diagnostics;
  const std::string source = sourceEdit_->toPlainText().toStdString();
  const std::string name = currentFilePath_.isEmpty()
                               ? std::string("<memory>")
                               : currentFilePath_.toStdString();

  pl0::CompileResult result =
      pl0::compile_source_text(name, source, options, diagnostics);

  populateDiagnostics(diagnostics);
  populateTokens(result.tokens);

  if (diagnostics.has_errors()) {
    lastResult_ = std::move(result);
    displayCompileFailure();
    if (!quiet) {
      statusBar()->showMessage(tr("编译失败"), 5000);
    }
    if (astImageLabel_) {
      astImageLabel_->clear();
      astImageLabel_->setMinimumSize(QSize(0, 0));
    }
    return false;
  }

  if (result.program) {
    updateAstDiagram(*result.program);
  }
  populateSymbols(result.symbols);
  populatePCode(result.code);
  vmOutputEdit_->clear();

  lastResult_ = std::move(result);
  if (!quiet) {
    statusBar()->showMessage(tr("编译成功"), 4000);
  }
  return true;
}

void MainWindow::executeCompiledProgram() {
  if (!lastResult_ || lastResult_->code.empty()) {
    QMessageBox::information(this, tr("无法运行"), tr("当前程序尚未成功编译。"));
    return;
  }

  pl0::RunnerOptions runOptions;
  runOptions.trace_vm = traceVmAction_->isChecked();
  runOptions.enable_bounds_check = boundsCheckAction_->isChecked();

  pl0::DiagnosticSink runtimeDiagnostics;
  std::istringstream input(stdinEdit_->toPlainText().toStdString());
  std::ostringstream output;

  StreamRedirector cinRedirect(std::cin, input.rdbuf());
  StreamRedirector coutRedirect(std::cout, output.rdbuf());

  auto vmResult = pl0::run_instructions(lastResult_->code, runtimeDiagnostics, runOptions);

  populateVmOutput(output.str());

  if (!runtimeDiagnostics.diagnostics().empty()) {
    QString text = diagnosticsEdit_->toPlainText();
    if (!text.isEmpty()) {
      text.append("\n");
    }
    text.append(tr("[运行时]\n"));
    for (const auto& diag : runtimeDiagnostics.diagnostics()) {
      text.append(QStringLiteral("%1 %2: %3 (%4)\n")
                      .arg(diagnosticLevelToString(diag.level))
                      .arg(static_cast<int>(diag.code))
                      .arg(QString::fromStdString(diag.message))
                      .arg(sourceRangeToString(diag.range)));
    }
    QString resultText = text.trimmed();
  if (resultText.isEmpty()) {
    resultText = tr("您的代码没有任何的报错喵 Ciallo～(∠・ω< )⌒★");
  }
  diagnosticsEdit_->setPlainText(resultText);
  }

  if (vmResult.success) {
    statusBar()->showMessage(tr("运行完成，最后结果 = %1").arg(vmResult.last_value),
                             4000);
  } else {
    statusBar()->showMessage(tr("运行过程中发生错误"), 5000);
  }
}

void MainWindow::displayCompileFailure() {
  symbolsTable_->setRowCount(0);
  pcodeEdit_->clear();
  vmOutputEdit_->clear();
  if (astImageLabel_) {
    astImageLabel_->clear();
    astImageLabel_->setMinimumSize(QSize(0, 0));
  }
}

void MainWindow::populateTokens(const std::vector<pl0::Token>& tokens) {
  tokensTable_->setRowCount(static_cast<int>(tokens.size()));
  for (int row = 0; row < static_cast<int>(tokens.size()); ++row) {
    const auto& token = tokens[static_cast<std::size_t>(row)];
    tokensTable_->setItem(row, 0,
                          new QTableWidgetItem(QString::number(row)));
    tokensTable_->setItem(row, 1,
                          new QTableWidgetItem(tokenKindToString(token.kind)));
    tokensTable_->setItem(row, 2,
                          new QTableWidgetItem(QString::fromStdString(token.lexeme)));
    tokensTable_->setItem(row, 3,
                          new QTableWidgetItem(sourceRangeToString(token.range)));
    QString value;
    if (token.number) {
      value = QString::number(*token.number);
    } else if (token.boolean) {
      value = *token.boolean ? tr("true") : tr("false");
    }
    tokensTable_->setItem(row, 4, new QTableWidgetItem(value));
  }
}

void MainWindow::populateSymbols(const std::vector<pl0::Symbol>& symbols) {
  symbolsTable_->setRowCount(static_cast<int>(symbols.size()));
  for (int row = 0; row < static_cast<int>(symbols.size()); ++row) {
    const auto& symbol = symbols[static_cast<std::size_t>(row)];
    symbolsTable_->setItem(row, 0,
                           new QTableWidgetItem(QString::fromStdString(symbol.name)));
    symbolsTable_->setItem(row, 1,
                           new QTableWidgetItem(symbolKindToString(symbol.kind)));
    symbolsTable_->setItem(row, 2,
                           new QTableWidgetItem(varTypeToString(symbol.type)));
    symbolsTable_->setItem(row, 3,
                           new QTableWidgetItem(QString::number(symbol.level)));
    symbolsTable_->setItem(row, 4,
                           new QTableWidgetItem(QString::number(symbol.address)));
    symbolsTable_->setItem(row, 5,
                           new QTableWidgetItem(QString::number(symbol.size)));
    symbolsTable_->setItem(row, 6,
                           new QTableWidgetItem(symbol.by_value ? tr("值传递")
                                                               : tr("引用")));
  }
}

void MainWindow::populatePCode(const pl0::InstructionSequence& code) {
  QStringList lines;
  lines.reserve(static_cast<int>(code.size()));
  for (const auto& instr : code) {
    lines.append(QString::fromStdString(pl0::to_string(instr)));
  }
  pcodeEdit_->setPlainText(lines.join(QStringLiteral("\n")));
}

void MainWindow::populateDiagnostics(const pl0::DiagnosticSink& diagnostics) {
  QString text;
  for (const auto& diag : diagnostics.diagnostics()) {
    text.append(QStringLiteral("%1 %2: %3 (%4)\n")
                    .arg(diagnosticLevelToString(diag.level))
                    .arg(static_cast<int>(diag.code))
                    .arg(QString::fromStdString(diag.message))
                    .arg(sourceRangeToString(diag.range)));
  }
  QString resultText = text.trimmed();
  if (resultText.isEmpty()) {
    resultText = tr("您的代码没有任何的报错喵 Ciallo～(∠・ω< )⌒★");
  }
  diagnosticsEdit_->setPlainText(resultText);
}

void MainWindow::populateVmOutput(const std::string& output) {
  QString text = QString::fromStdString(output);
  QStringList lines = text.split(QChar('\n'), Qt::KeepEmptyParts);
  QStringList processed;
  QStringList programOutputs;
  const bool traceEnabled = traceVmAction_ && traceVmAction_->isChecked();
  QRegularExpression instructionPattern(QStringLiteral("^\\s*(\\d+):\\s*(.*)$"));

  for (int i = 0; i < lines.size(); ++i) {
    QString original = lines.at(i);
    QString trimmed = original.trimmed();
    if (trimmed.isEmpty()) {
      continue;
    }

    QRegularExpressionMatch match = instructionPattern.match(trimmed);
    if (match.hasMatch()) {
      QString instruction = match.captured(2).trimmed();
      if (traceEnabled && instruction.startsWith(QStringLiteral("opr 0 write"))) {
        if (i + 1 < lines.size()) {
          QString nextLine = lines.at(i + 1).trimmed();
          QRegularExpressionMatch nextMatch = instructionPattern.match(nextLine);
          if (!nextLine.isEmpty() && !nextMatch.hasMatch()) {
            processed.append(instruction);
            processed.append(QStringLiteral("output: %1").arg(nextLine));
            programOutputs.append(nextLine);
            ++i;
            continue;
          }
        }
      }
      processed.append(instruction);
    } else {
      processed.append(trimmed);
      if (traceEnabled) {
        programOutputs.append(trimmed);
      }
    }
  }

  if (traceEnabled && !programOutputs.isEmpty()) {
    processed.append(QString());
    processed.append(QStringLiteral("output: %1")
                         .arg(programOutputs.join(QStringLiteral(", "))));
  }

  vmOutputEdit_->setPlainText(processed.join(QStringLiteral("\n")).trimmed());
}

QString MainWindow::tokenKindToString(pl0::TokenKind kind) const {
  return QString::fromStdString(pl0::to_string(kind));
}

QString MainWindow::symbolKindToString(pl0::SymbolKind kind) const {
  switch (kind) {
    case pl0::SymbolKind::Constant:
      return tr("常量");
    case pl0::SymbolKind::Variable:
      return tr("变量");
    case pl0::SymbolKind::Procedure:
      return tr("过程");
    case pl0::SymbolKind::Parameter:
      return tr("参数");
    case pl0::SymbolKind::Array:
      return tr("数组");
  }
  return tr("未知");
}

QString MainWindow::varTypeToString(pl0::VarType type) const {
  switch (type) {
    case pl0::VarType::Integer:
      return tr("整数");
    case pl0::VarType::Boolean:
      return tr("布尔");
  }
  return tr("未知");
}

void MainWindow::markDocumentDirty() {
  documentDirty_ = true;
  lastResult_.reset();
  updateWindowTitle();
}

void MainWindow::updateWindowTitle() {
  QString title = tr("编译原理展示");
  if (documentDirty_) {
    title += tr(" *");
  }
  setWindowTitle(title);
}
