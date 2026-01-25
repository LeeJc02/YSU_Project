#include "withdrawal.h"
#include "ui_withdrawal.h"

withdrawal::withdrawal(BankCard &c, QWidget *parent) :
        QWidget(parent), card(&c),
        ui(new Ui::withdrawal) {
    ui->setupUi(this);

    connect(ui->pushButton_2, &QPushButton::clicked, this, &withdrawal::close);
}

withdrawal::~withdrawal() {
    delete ui;
}

void withdrawal::on_pushButton_clicked() {

    double b = card->GetBalance();

    QString text = ui->lineEdit->text();
    bool ok;
    double number = text.toDouble(&ok);
    if (ok)
        if (b >= number) {
            card->balance = b - number;
            warnWidget("取款成功!");
        } else warnWidget("余额不足请重新输入!");
    else
        warnWidget("取款失败请重新输入!");
}

void withdrawal::warnWidget(QString s) {
    auto warn = new warning(s);
    warn->show();
}
