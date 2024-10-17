//
// Created by Lee on 2024/3/30.
//

#ifndef CPP_PROJECT4_0_ONE_CLASS_HPP
#define CPP_PROJECT4_0_ONE_CLASS_HPP

#include <QWidget>
#include <string>
#include "ui_one_class.h"
#include <QRegExpValidator>
#include <QDebug>
#include "../../Class_ATM/ATM.h"
#include "../../Class_Card/Card.h"
#include "../menu_class/widget.h"
#include "../warning_class/warning.h"
#include "../registration_class/registration.h"
#include "../csv/csv.h"

QT_BEGIN_NAMESPACE
namespace Ui { class one_class; }
QT_END_NAMESPACE

class one_class : public QWidget {
Q_OBJECT

public:
    explicit one_class(ATM &a, QWidget *parent = nullptr);

    ~one_class() override;

private slots:
    void writeoff();
    void user_register();
private:
    void warningWidget(const QString &s);
    void menuWidget(BankCard &card);
    void userFind();
    void rootWidget();
    void save();

    Ui::one_class *ui;
    QString key_word;
    QString user_number;
    ATM *atm;
    BankCard *card;
    Widget *menu;
};


#endif //CPP_PROJECT4_0_ONE_CLASS_HPP
