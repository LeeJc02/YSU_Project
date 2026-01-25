#include <QApplication>
#include <QPushButton>
#include "Class_QT/two_class/two_class.hpp"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
//    ATM system("./user_data.csv");
//    system.systemLogin();
//    one_class a;
//    a.show();
    two_class start;
    start.show();
    return QApplication::exec();
}


