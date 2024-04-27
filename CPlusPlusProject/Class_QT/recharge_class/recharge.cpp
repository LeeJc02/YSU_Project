#include "recharge.h"
#include "ui_recharge.h"

recharge::recharge(BankCard &c, QWidget *parent) :
        QWidget(parent), card(&c),
        ui(new Ui::recharge) {
    ui->setupUi(this);

    connect(ui->pushButton_2, &QPushButton::clicked, this, &recharge::close);
    connect(ui->pushButton, &QPushButton::clicked, this, &recharge::on_pushButton_clicked);
}

recharge::~recharge() {
    delete ui;
}

void recharge::on_pushButton_clicked() {

    double b = card->GetBalance();

    QString text = ui->lineEdit->text();
    bool ok;
    double number = text.toDouble(&ok);
    if (ok) {
        card->balance = b + number;
        warnWidget("充值成功!");
    } else
        warnWidget("充值失败请重新输入!");
}

void recharge::warnWidget(QString s) {
    auto warn = new warning(s);
    warn->show();
}