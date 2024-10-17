#include "logan.h"
#include "ui_logan.h"
#include "../menu_class/widget.h"

logan::logan(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::logan)
{
    ui->setupUi(this);
}

logan::~logan()
{
    delete ui;


}

void logan::on_pushButton_clicked()
{
this->hide();
    parentWidget()->show();


}
