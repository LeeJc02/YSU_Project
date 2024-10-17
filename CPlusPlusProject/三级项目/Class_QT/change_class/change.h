#ifndef CHANGE_H
#define CHANGE_H

#include <QWidget>
#include "../../Class_Card/Card.h"
#include "../warning_class/warning.h"
#include <QString>

namespace Ui {
class change;
}

class change : public QWidget
{
    Q_OBJECT

public:
    explicit change(BankCard &c, QWidget *parent = nullptr);
    ~change();

private slots:

    void on_pushButton_clicked();

private:

    void warnWidget(const QString &s);

    Ui::change *ui;
    QString expression;
    BankCard *card;
};

#endif // CHANGE_H
