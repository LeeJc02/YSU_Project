//
// Created by Lee on 2024/4/18.
//

#ifndef CPP_PROJECT4_0_THREE_CLASS_HPP
#define CPP_PROJECT4_0_THREE_CLASS_HPP

#include <QWidget>
#include <iostream>
#include <streambuf>
#include "../../Class_Dos/DOS.h"
#include <QPushButton>
#include <QEvent>
#include <QKeyEvent>
#include <QDebug>
#include "../../Class_ATM/ATM.h"
#include "../../Class_Card/Card.h"
#include <QProcess>
#include <QShortcut>
#include <QVBoxLayout>
#include <sstream>
#include <QStateMachine>
#include <QSignalTransition>
#include <QTextCodec>

QT_BEGIN_NAMESPACE
namespace Ui { class three_class; }
QT_END_NAMESPACE

class three_class : public QWidget {
Q_OBJECT

public:
    explicit three_class(ATM &a, QWidget *parent = nullptr);

    ~three_class() override;

private slots:

    void executeFunction(const QString &input);

    void saveQuit();
//    QSignalTransition *createTransition(QObject *sender, QState *targetState);

signals:

    void enterPressed(const QString &s);


protected:
    void displayMessage(const QString &s);

    static bool isValidPassword(const string &password);//检测银行卡密码是否有效的函数

    bool eventFilter(QObject *obj, QEvent *event) override;

    static QString findDifference(const QString &str1, const QString &str2);

    streambuf *buf;
    ATM *atm;
    Ui::three_class *ui;
    QString oldText;
    QString newText;
    BankCard *card;
    QString user_name;
    QString user_number;
    QString key_word;
    QString old_key;
    QString new_key;
    QString re_key;
    enum class State {
        EnterCardNumber,
        EnterPin,
        Transaction,
        Withdraw,
        Recharge,
        Start,
        ChangeOld,
        ChangeNew,
        ChangeRe
        // 可以添加更多状态
    };
    State currentState;
};


#endif //CPP_PROJECT4_0_THREE_CLASS_HPP
