//
// Created by Lee on 2024/3/30.
//

#ifndef CPP_PROJECT4_0_TWO_CLASS_HPP
#define CPP_PROJECT4_0_TWO_CLASS_HPP

#include <QWidget>
#include "ui_two_class.h"
#include "../../Class_ATM/ATM.h"
#include "../one_class/one_class.hpp"
#include "../three_class/three_class.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class two_class; }
QT_END_NAMESPACE

class two_class : public QWidget {
Q_OBJECT


public:
    explicit two_class(QWidget *parent = nullptr);

    ~two_class() override;

private slots:

    static void quit() ;
    void save();
    void newWidget_DOS();
    void newWidget_GUI();
private:
//    void login();
//    void DOS_func();
//    void processFinished();
    Ui::two_class *ui;
    ATM *atm;
    BankCard *card;
};


#endif //CPP_PROJECT4_0_TWO_CLASS_HPP
