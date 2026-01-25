#include "widget.h"
#include "ui_widget.h"
#include "../enquiry_class//enquiry.h"

Widget::Widget(BankCard &c, QWidget *parent)
        : QWidget(parent), ui(new Ui::Widget), card(&c) {
    ui->setupUi(this);

    connect(ui->change_2, &QPushButton::clicked, this, &QWidget::close);

}

Widget::~Widget() {
    delete ui;
    delete wenquiry;
    delete wlogan;
    delete wrecharge;
    delete wchange;
    delete wwithdrawal;
}


void Widget::on_Enquiry_clicked() {
    double b = card->GetBalance();
    wenquiry = new Enquiry(card->GetBalance(), this);

    qDebug() << QString::number(b);
    wenquiry->show();

}

void Widget::on_logan_clicked() {
    wlogan = new logan(this);
    wlogan->show();
}

void Widget::on_Recharge_clicked() {
    wrecharge = new recharge(*card, this);
    wrecharge->show();
}

void Widget::on_Withdrawal_clicked() {
    wwithdrawal = new withdrawal(*card, this);
    wwithdrawal->show();
}

void Widget::on_change_clicked() {
    wchange = new change(*card, this);
    wchange->show();
}

