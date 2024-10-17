#include "enquiry.h"
#include "ui_enquiry.h"

Enquiry::Enquiry(double balance, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Enquiry)
{
    ui->setupUi(this);
    ui->lineEdit->setText(QString::number(balance));
}

Enquiry::~Enquiry()
{
    delete ui;
}

void Enquiry::on_pushButton_clicked()
{
    this->hide();
    parentWidget()->show();
}
