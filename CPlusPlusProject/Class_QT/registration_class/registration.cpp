#include "registration.h"
#include "ui_registration.h"

registration::registration(ATM &a, QWidget *parent) :
    QWidget(parent),atm(&a),
    ui(new Ui::registration)
{
    ui->setupUi(this);
    connect(ui->pushButton, &QPushButton::clicked, this, &registration::enter);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &registration::close);
}

registration::~registration()
{
    delete ui;
}

void registration::warned(QString s) {
    auto warn = new warning(s);
    warn->show();
}

void registration::enter() {
    auto name = ui->lineEdit->text();
    auto key = ui->lineEdit_5->text();
    auto key_2 = ui->lineEdit_6->text();

    // 使用 QRandomGenerator 生成随机数
    qint64 randomNumber = QRandomGenerator::global()->generate64();

    // 将随机数转换为字符串，并截取前19位作为随机序列
    auto randomSequence = QString::number(randomNumber).left(19);

    atm->userLogin(randomSequence.toStdString(), key.toStdString(), card);

    if(key != key_2 || key.length() != 6 || card != nullptr) {
        warned("输入错误请重新输入!");
        return;
    }


    card = new BankCard(randomSequence.toStdString(), key.toStdString(), name.toStdString(), 0);
    atm->userRegister(randomSequence.toStdString(), key.toStdString(), *card);
    warned("创建成功!");
}


