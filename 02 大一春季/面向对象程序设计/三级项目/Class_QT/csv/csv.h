#ifndef CSV_H
#define CSV_H

#include <QWidget>
#include <QDebug>
#include <QFile>
#include <QStandardItemModel>
#include <QTableView>
#include <QTextStream>
#include <QStringList>
#include "../../Class_Card/Card.h"
#include "../../Class_ATM/ATM.h"
#include "../change_class/change.h"

namespace Ui {
    class csv;
}

class csv : public QWidget {
Q_OBJECT

public:
    explicit csv(ATM &a, BankCard &c, QWidget *parent = nullptr);

    ~csv();


    void loadCSVFile(); // 新添加的函数声明
private:
    void on_change_clicked();

    Ui::csv *ui;
    ATM *atm;
    BankCard *card;

};

#endif // CSV_H
