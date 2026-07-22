#include <QApplication>
#include <QStyle>

#include "MainWindow.hpp"

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);
  QApplication::setApplicationDisplayName(QStringLiteral("PL/0 Studio"));
  QApplication::setOrganizationName(QStringLiteral("PL0"));

  if (QApplication::style()->objectName().compare("fusion", Qt::CaseInsensitive) != 0) {
    QApplication::setStyle("fusion");
  }

  MainWindow window;
  window.show();
  return QApplication::exec();
}
