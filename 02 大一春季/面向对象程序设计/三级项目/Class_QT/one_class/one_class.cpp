//
// Created by Lee on 2024/3/30.
//

// You may need to build the project (run Qt uic code generator) to get "ui_one_class.h" resolved

#include "one_class.hpp"

one_class::one_class(ATM &a, QWidget *parent) :
        QWidget(parent), ui(new Ui::one_class), atm(&a) {
    ui->setupUi(this);

//    atm = new ATM("D:\\Using\\CLion\\CLionProject\\CPP_Project6.0\\user_data.csv");

    // 双击 -> 账户文本框光标跳转开头
    connect(ui->lineEdit, &QLineEdit::selectionChanged, [this]() {
        ui->lineEdit->setCursorPosition(0);
    });

    // 双击 -> 密码文本框光标跳转开头
    connect(ui->lineEdit_2, &QLineEdit::selectionChanged, [this]() {
        ui->lineEdit_2->setCursorPosition(0);
    });

    // 设置密码要求是数字或字母, 长度为6
    ui->lineEdit_2->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z0-9]+$")));
    ui->lineEdit_2->setMaxLength(6);

//    // 输入账户 -> 记录账户
//    connect(ui->lineEdit, &QLineEdit::textEdited, [&](const QString &text) {
//        QString currentText = ui->lineEdit->text();
//        if (currentText.length() <= this->user_number.length()) {
//            this->user_number.remove(this->user_number.size() - 1, 1);
//            return;
//        }
//        this->user_number += currentText[currentText.length() - 1];
//    });

//    // 账户Tab -> 光标跳转密码文本框开头
//    connect(ui->lineEdit, &QLineEdit::returnPressed, [this]() {
//        ui->lineEdit_2->setCursorPosition(0);
//        ui->lineEdit_2->setFocus();
//    });

    // 账户Enter -> 光标跳转密码文本框开头
    connect(ui->lineEdit, &QLineEdit::editingFinished, [this]() {
        ui->lineEdit_2->setCursorPosition(0);
        ui->lineEdit_2->setFocus();
    });

    // 密码Enter -> 执行确认键
    connect(ui->lineEdit, &QLineEdit::returnPressed, ui->pushButton, &QPushButton::click);

    // 输入密码隐藏
    connect(ui->lineEdit_2, &QLineEdit::textEdited, [&](const QString &text) {
        QString currentText = ui->lineEdit_2->text();
        if (currentText.length() <= this->key_word.length()) {
            this->key_word.remove(this->key_word.size() - 1, 1);
            return;
        }
        this->key_word += currentText[currentText.length() - 1];
    });

    // 勾选 -> 显示密码明文/密文
    connect(ui->checkBox, &QCheckBox::clicked, [&](bool checked) {
        ui->lineEdit_2->setEchoMode(checked ? QLineEdit::EchoMode::Normal : QLineEdit::EchoMode::Password);
    });

    connect(ui->pushButton, &QPushButton::clicked, [this]() {
        userFind();
        if (card == nullptr) warningWidget("录入信息错误请重新输入!");
        else if(card->isRoot()) rootWidget();
        else menuWidget(*card);
    });


    // 调试用户名和密码
    connect(ui->pushButton, &QPushButton::clicked, [this]() {
//        qDebug() << "key word" << this->key_word;
//        qDebug() << "user number" << this->user_number;
    });


//    QLabel *label = new QLabel("Transparent Background");
//    label->setStyleSheet("background-color: rgba(0, 0, 0, 0);"); // 将背景颜色设置为透明

    connect(ui->pushButton_2, &QPushButton::clicked, this, &one_class::save);
    connect(ui->writeoff, &QPushButton::clicked, this, &one_class::writeoff);
    connect(ui->register_2, &QPushButton::clicked, this, &one_class::user_register);

}

void one_class::save() {
    atm->userExit();
    this->close();
}

void one_class::writeoff() {
    userFind();
//    card->GetCardNo();
//    qDebug() << QString::fromStdString(card->user_name);
    if (card == nullptr)
        warningWidget("录入信息错误请重新输入!");
    else if (card->isRoot())
        warningWidget("Root账户无法注销!");
    else {
//        qDebug() << card->isRoot();
//        qDebug() << ui->lineEdit->text();
//        qDebug() << ui->lineEdit_2->text();
        atm->userLogout(ui->lineEdit->text().toStdString(), ui->lineEdit_2->text().toStdString());
        card = nullptr;
        warningWidget("用户注销成功");
    }
}

void one_class::userFind() {
    QString temp = ui->lineEdit->text();
    for (auto i: temp)
        if (!(i >= '0' && i <= '9')) {
            warningWidget("录入信息错误请重新输入!");
            break;
        }

    user_number = temp;
    temp.clear();
    temp = ui->lineEdit_2->text();
    for (auto i: temp)
        if (!(i >= '0' && i <= '9' || i >= 'a' && i <= 'z' || i >= 'A' && i <= 'Z')) {
            warningWidget("录入信息错误请重新输入!");
            break;
        }

    key_word = temp;

//        qDebug() << user_number;
//        qDebug() << key_word;
    string name = user_number.toStdString();
    string key = key_word.toStdString();
//    for(const auto &i:atm->user_data)
//        qDebug() << QString::fromStdString(i.first.first) << " " << QString::fromStdString(i.first.second) << " " << QString::fromStdString(i.second.user_name);
    atm->userLogin(name, key, card);
}

void one_class::user_register() {
    auto regis = new registration(*atm, this);
    regis->show();
}

void one_class::menuWidget(BankCard &card) {
    menu = new Widget(card, this);
    menu->show();
}

void one_class::warningWidget(const QString &s) {
    ui->lineEdit_2->clear();
    ui->lineEdit->clear();
    auto warn = new warning(s);
    warn->show();
}

one_class::~one_class() {
    delete ui;
}

void one_class::rootWidget() {
    auto r = new csv(*atm, *card, this);
    r->show();
}