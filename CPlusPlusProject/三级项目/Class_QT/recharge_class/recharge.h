#ifndef RECHARGE_H
#define RECHARGE_H

#include <QWidget>
#include "../warning_class/warning.h"
#include "../../Class_Card/Card.h"

namespace Ui {
class recharge;
}

class recharge : public QWidget
{
    Q_OBJECT

public:
    explicit recharge(BankCard &c, QWidget *parent = nullptr);
    ~recharge();

private slots:
    void on_pushButton_clicked();
private:
    void warnWidget(QString s);
    Ui::recharge *ui;
    BankCard *card;
};

#endif // RECHARGE_H
