#pragma once

#include <QPlainTextEdit>

class LineNumberArea;

class CodeEditor : public QPlainTextEdit {
  Q_OBJECT

 public:
  explicit CodeEditor(QWidget* parent = nullptr);

  int lineNumberAreaWidth() const;
  void setLineNumberFont(const QFont& font);
  void paintLineNumbers(QPaintEvent* event);

 protected:
  void resizeEvent(QResizeEvent* event) override;

 private Q_SLOTS:
  void updateLineNumberAreaWidth(int newBlockCount = 0);
  void highlightCurrentLine();
  void updateLineNumberArea(const QRect& rect, int dy);

 private:
  QWidget* lineNumberArea_ = nullptr;
  QFont lineNumberFont_;
};

class LineNumberArea : public QWidget {
 public:
  explicit LineNumberArea(CodeEditor* editor);

  QSize sizeHint() const override;

 protected:
  void paintEvent(QPaintEvent* event) override;

 private:
  CodeEditor* editor_;
};
