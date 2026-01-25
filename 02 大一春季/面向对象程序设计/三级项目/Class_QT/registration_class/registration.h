#ifndef REGISTRATION_H
#define REGISTRATION_H

#include <QWidget>
#include "../warning_class/warning.h"
#include "../../Class_Card/Card.h"
#include "../../Class_ATM/ATM.h"
#include <QRandomGenerator>

namespace Ui {
class registration;
}

class registration : public QWidget
{
    Q_OBJECT

public:
    explicit registration(ATM &a, QWidget *parent = nullptr);
    ~registration();

private slots:
    void warned(QString s);
    void enter();
private:
    Ui::registration *ui;
    BankCard *card;
    ATM *atm;
};

#endif // REGISTRATION_H
