#include "damo.h"
#include "ui_damo.h"

damo::damo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::damo)
{
    ui->setupUi(this);
}

damo::~damo()
{
    delete ui;
}
