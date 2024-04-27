//
// Created by Lee on 2024/3/30.
//

// You may need to build the project (run Qt uic code generator) to get "ui_two_class.h" resolved

#include "two_class.hpp"

two_class::two_class(QWidget *parent) :
        QWidget(parent), ui(new Ui::two_class) {
    ui->setupUi(this);
    atm = new ATM("D:\\Using\\CLion\\CLionProject\\CPP_Project6.0\\user_data.csv");

    connect(ui->pushButton, &QPushButton::clicked, this, &two_class::newWidget_DOS);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &two_class::newWidget_GUI);
    connect(ui->pushButton_3, &QPushButton::clicked, this, &two_class::save);
    connect(ui->pushButton_3, &QPushButton::clicked, this, &two_class::quit);


}

void two_class::quit() {
    qApp->quit();
}

void two_class::save() {
    atm->userExit();
}

void two_class::newWidget_DOS() {
    auto *DOS = new three_class(*atm, this);
    DOS->show();
}

void two_class::newWidget_GUI() {
    auto *GUI = new one_class(*atm, this);
    GUI->show();
}

two_class::~two_class() {
    delete ui;
    delete atm;
}
