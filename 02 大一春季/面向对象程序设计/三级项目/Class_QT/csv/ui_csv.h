/********************************************************************************
** Form generated from reading UI file 'csv.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CSV_H
#define UI_CSV_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_csv
{
public:
    QLabel *label;
    QTableView *tableView;
    QLabel *label_2;
    QPushButton *back;
    QPushButton *change;

    void setupUi(QWidget *csv)
    {
        if (csv->objectName().isEmpty())
            csv->setObjectName(QString::fromUtf8("csv"));
        csv->resize(1100, 600);
        csv->setMinimumSize(QSize(1100, 600));
        csv->setMaximumSize(QSize(1100, 600));
        label = new QLabel(csv);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(0, 0, 1101, 601));
        label->setStyleSheet(QString::fromUtf8("background-image: url(:/images/resource/background.png);"));
        tableView = new QTableView(csv);
        tableView->setObjectName(QString::fromUtf8("tableView"));
        tableView->setGeometry(QRect(250, 160, 561, 351));
        tableView->setStyleSheet(QString::fromUtf8("background-image: url(:/images/resource/write.jpg);"));
        label_2 = new QLabel(csv);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(250, 160, 561, 351));
        label_2->setStyleSheet(QString::fromUtf8("background-image: url(:/images/resource/write.jpg);"));
        back = new QPushButton(csv);
        back->setObjectName(QString::fromUtf8("back"));
        back->setGeometry(QRect(890, 330, 151, 61));
        QFont font;
        font.setFamily(QString::fromUtf8("Arial"));
        font.setPointSize(14);
        back->setFont(font);
        change = new QPushButton(csv);
        change->setObjectName(QString::fromUtf8("change"));
        change->setGeometry(QRect(890, 430, 151, 61));
        change->setFont(font);

        retranslateUi(csv);

        QMetaObject::connectSlotsByName(csv);
    } // setupUi

    void retranslateUi(QWidget *csv)
    {
        csv->setWindowTitle(QCoreApplication::translate("csv", "Form", nullptr));
        label->setText(QString());
        label_2->setText(QString());
        back->setText(QCoreApplication::translate("csv", "\350\277\224\345\233\236", nullptr));
        change->setText(QCoreApplication::translate("csv", "\344\277\256\346\224\271root\345\257\206\347\240\201", nullptr));
    } // retranslateUi

};

namespace Ui {
    class csv: public Ui_csv {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CSV_H
