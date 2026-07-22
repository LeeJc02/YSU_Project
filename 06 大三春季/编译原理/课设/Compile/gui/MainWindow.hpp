#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <QPlainTextEdit>
#include <QVector>

#include <QMainWindow>
#include <QPixmap>

#include "pl0/Driver.hpp"

QT_BEGIN_NAMESPACE
class QAction;
class QCheckBox;
class QSplitter;
class QStatusBar;
class QTableWidget;
class QTabWidget;
class QLabel;
class QScrollArea;
QT_END_NAMESPACE

class CodeEditor;

namespace pl0 {
struct Token;
struct Program;
struct Block;
struct Expression;
}

class QGraphicsOpacityEffect;

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget* parent = nullptr);

 private:
  void resizeEvent(QResizeEvent* event) override;

 private Q_SLOTS:
  void openFile();
  void saveFile();
  void saveFileAs();
  void compileSource();
  void runProgram();
  void compileAndRun();

 private:
  void setupUi();
  void setupMenus();
  void setupConnections();
  void updateWindowTitle();
  void markDocumentDirty();
  void updateFonts();
  void updateAstDiagram(const pl0::Program& program);
  void exportAstImage(const QPixmap& pixmap) const;
  QPixmap createAstPixmap(const pl0::Program& program, const QFont& font) const;
  void relayoutOverlays();

  void populateTokens(const std::vector<pl0::Token>& tokens);
  void populateSymbols(const std::vector<pl0::Symbol>& symbols);
  void populatePCode(const pl0::InstructionSequence& code);
  void populateDiagnostics(const pl0::DiagnosticSink& diagnostics);
  void populateVmOutput(const std::string& output);

  bool compileInternal(bool quiet = false);
  void executeCompiledProgram();
  void displayCompileFailure();

  QString tokenKindToString(pl0::TokenKind kind) const;
  QString symbolKindToString(pl0::SymbolKind kind) const;
  QString varTypeToString(pl0::VarType type) const;

  bool promptToSave();
  bool saveToPath(const QString& path);

  QString currentFilePath_;
  bool documentDirty_ = false;

  class CodeEditor* sourceEdit_ = nullptr;
  class CodeEditor* stdinEdit_ = nullptr;
  class CodeEditor* pcodeEdit_ = nullptr;
  class CodeEditor* diagnosticsEdit_ = nullptr;
  class CodeEditor* vmOutputEdit_ = nullptr;
  QTableWidget* tokensTable_ = nullptr;
  QTableWidget* symbolsTable_ = nullptr;
  QTabWidget* rightTabs_ = nullptr;
  QScrollArea* astImageScroll_ = nullptr;
  QLabel* astImageLabel_ = nullptr;

  QAction* boundsCheckAction_ = nullptr;
  QAction* traceVmAction_ = nullptr;
  QAction* compileAction_ = nullptr;
  QAction* runAction_ = nullptr;
  QAction* compileRunAction_ = nullptr;
  QAction* openAction_ = nullptr;
  QAction* saveAsAction_ = nullptr;
  QAction* exitAction_ = nullptr;
  QAction* saveAction_ = nullptr;
  QMenu* fileMenu_ = nullptr;
  QMenu* buildMenu_ = nullptr;
  QMenu* optionsMenu_ = nullptr;
  QToolBar* mainToolBar_ = nullptr;

  std::optional<pl0::CompileResult> lastResult_;
  QLabel* watermarkLabel_ = nullptr;
  QLabel* backgroundLabel_ = nullptr;
  QGraphicsOpacityEffect* backgroundOpacityEffect_ = nullptr;
  QPixmap backgroundPixmap_;
  QSize initialWindowSize_;
  QFont baseMonospaceFont_;
  QFont baseUiFont_;
  QFont originalUiFont_;
  QVector<QWidget*> toolbarWidgets_;
};
