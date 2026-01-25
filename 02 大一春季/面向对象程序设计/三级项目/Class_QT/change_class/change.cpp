#include "change.h"
#include "ui_change.h"

change::change(BankCard &c, QWidget *parent) :
        QWidget(parent), card(&c),
        ui(new Ui::change) {
    ui->setupUi(this);

    ui->lineEdit_2->setMaxLength(6);
    connect(ui->lineEdit_2, &QLineEdit::selectionChanged, [this]() {
        ui->lineEdit_2->setCursorPosition(0);
    });

    ui->lineEdit->setMaxLength(6);
    connect(ui->lineEdit, &QLineEdit::selectionChanged, [this]() {
        ui->lineEdit->setCursorPosition(0);
    });


    ui->lineEdit_3->setMaxLength(6);
    connect(ui->lineEdit_3, &QLineEdit::selectionChanged, [this]() {
        ui->lineEdit_3->setCursorPosition(0);
    });

    connect(ui->pushButton_2, &QPushButton::clicked, this, &change::close);
//    connect(ui->pushButton, &QPushButton::clicked, this, &change::on_pushButton_clicked);
}

change::~change() {
    delete ui;
}

void change::on_pushButton_clicked() {


    QString text1 = ui->lineEdit->text();
    QString text2 = ui->lineEdit_2->text();
    QString text3 = ui->lineEdit_3->text();


    auto old_word = text1.toStdString();
    auto new_word1 = text2.toStdString();
    auto new_word2 = text3.toStdString();
    if (card->changePassword(old_word, new_word1, new_word2)){
        qDebug() << "成功";
        warnWidget("修改密码成功!");
        return;
    }

    qDebug() << "失败";
    warnWidget("输入有误请重新输入");


}

void change::warnWidget(const QString &s) {
    ui->lineEdit->clear();
    ui->lineEdit_2->clear();
    ui->lineEdit_3->clear();
    auto warn = new warning(s);
    warn->show();
}