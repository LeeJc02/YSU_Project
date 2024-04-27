//
// Created by Lee on 2024/4/21.
//

// You may need to build the project (run Qt uic code generator) to get "ui_warning.h" resolved

#include "warning.h"
#include "ui_warning.h"


warning::warning(const QString &node, QWidget *parent) :
        QDialog(parent), ui(new Ui::warning) {
    ui->setupUi(this);
    ui->label->setText(node);
    connect(ui->pushButton, &QPushButton::clicked, this, &QWidget::close);
}

warning::~warning() {
    delete ui;
}
