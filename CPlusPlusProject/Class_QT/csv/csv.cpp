#include "csv.h"
#include "ui_csv.h"


csv::csv(ATM &a, BankCard &c, QWidget *parent) :
        QWidget(parent), atm(&a), card(&c),
        ui(new Ui::csv) {
    ui->setupUi(this);
    loadCSVFile();

    connect(ui->back, &QPushButton::clicked, this, &csv::close);
    connect(ui->change, &QPushButton::clicked, this, &csv::on_change_clicked);
}

void csv::loadCSVFile() {
    QFile file(QString::fromStdString(atm->user_file_path));

//    QFile file("D:\\Using\\CLion\\CLionProject\\CPP_Project6.0\\user_data.csv");
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "无法打开文件";
        return;
    }

    QTextStream in(&file);
    in.setCodec("UTF-8"); // 设置文件编码为 UTF-8
    QStringList lines;
    while (!in.atEnd()) {
        QString line = in.readLine();
        lines.append(line);
    }
    file.close();

    auto *model = new QStandardItemModel(this);
    model->setRowCount(lines.size());
    model->setColumnCount(4);
    model->setRowCount(5);
    for (int row = 0; row < lines.size(); ++row) {
        QStringList fields = lines[row].split(",");
        for (int col = 0; col < fields.size(); ++col) {
            model->setItem(row, col, new QStandardItem(fields[col]));
        }
    }

    ui->tableView->setModel(model);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

csv::~csv() {
    delete ui;
}

void csv::on_change_clicked() {
    auto ch = new change(*card, this);
    ch->show();
}