#ifndef WITHDRAWAL_H
#define WITHDRAWAL_H

#include <QWidget>
#include "../warning_class/warning.h"
#include "../../Class_Card/Card.h"

namespace Ui {
class withdrawal;
}

class withdrawal : public QWidget
{
    Q_OBJECT

public:
    explicit withdrawal(BankCard &c, QWidget *parent = nullptr);
    ~withdrawal();

private slots:

    void on_pushButton_clicked();

private:
    void warnWidget(QString s);

    Ui::withdrawal *ui;
    BankCard *card;
};

#endif // WITHDRAWAL_H
