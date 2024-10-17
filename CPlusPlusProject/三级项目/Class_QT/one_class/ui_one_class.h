/********************************************************************************
** Form generated from reading UI file 'one_class.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ONE_CLASS_H
#define UI_ONE_CLASS_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_one_class
{
public:
    QLabel *label;
    QLineEdit *lineEdit;
    QLabel *label_2;
    QPushButton *pushButton;
    QLabel *label_4;
    QLineEdit *lineEdit_2;
    QPushButton *pushButton_2;
    QLabel *label_3;
    QPushButton *register_2;
    QPushButton *writeoff;
    QCheckBox *checkBox;

    void setupUi(QWidget *one_class)
    {
        if (one_class->objectName().isEmpty())
            one_class->setObjectName(QString::fromUtf8("one_class"));
        one_class->resize(1100, 600);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(one_class->sizePolicy().hasHeightForWidth());
        one_class->setSizePolicy(sizePolicy);
        one_class->setStyleSheet(QString::fromUtf8("background-image: url(:/images/resource/background.png);"));
        label = new QLabel(one_class);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(0, 0, 1101, 601));
        label->setStyleSheet(QString::fromUtf8("background-image: url(:/images/resource/background.png);"));
        lineEdit = new QLineEdit(one_class);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setGeometry(QRect(280, 220, 541, 51));
        QFont font;
        font.setFamily(QString::fromUtf8("Arial"));
        font.setPointSize(18);
        lineEdit->setFont(font);
        label_2 = new QLabel(one_class);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(140, 230, 101, 41));
        QFont font1;
        font1.setFamily(QString::fromUtf8("Arial"));
        font1.setPointSize(20);
        label_2->setFont(font1);
        pushButton = new QPushButton(one_class);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(860, 430, 141, 61));
        pushButton->setFont(font1);
        label_4 = new QLabel(one_class);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(140, 340, 101, 41));
        label_4->setFont(font1);
        lineEdit_2 = new QLineEdit(one_class);
        lineEdit_2->setObjectName(QString::fromUtf8("lineEdit_2"));
        lineEdit_2->setGeometry(QRect(280, 340, 541, 51));
        lineEdit_2->setFont(font);
        lineEdit_2->setEchoMode(QLineEdit::Password);
        pushButton_2 = new QPushButton(one_class);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setGeometry(QRect(860, 510, 141, 61));
        pushButton_2->setFont(font1);
        label_3 = new QLabel(one_class);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(350, 110, 361, 41));
        QFont font2;
        font2.setFamily(QString::fromUtf8("Arial"));
        font2.setPointSize(26);
        label_3->setFont(font2);
        register_2 = new QPushButton(one_class);
        register_2->setObjectName(QString::fromUtf8("register_2"));
        register_2->setGeometry(QRect(680, 430, 141, 61));
        register_2->setFont(font1);
        writeoff = new QPushButton(one_class);
        writeoff->setObjectName(QString::fromUtf8("writeoff"));
        writeoff->setGeometry(QRect(680, 510, 141, 61));
        writeoff->setFont(font1);
        checkBox = new QCheckBox(one_class);
        checkBox->setObjectName(QString::fromUtf8("checkBox"));
        checkBox->setGeometry(QRect(870, 360, 91, 19));
        label->raise();
        label_2->raise();
        label_4->raise();
        label_3->raise();
        lineEdit->raise();
        lineEdit_2->raise();
        pushButton->raise();
        pushButton_2->raise();
        register_2->raise();
        writeoff->raise();
        checkBox->raise();

        retranslateUi(one_class);

        QMetaObject::connectSlotsByName(one_class);
    } // setupUi

    void retranslateUi(QWidget *one_class)
    {
        one_class->setWindowTitle(QCoreApplication::translate("one_class", "YSU\351\223\266\350\241\214---\347\224\250\346\210\267\347\231\273\345\275\225", nullptr));
        label->setText(QString());
        lineEdit->setInputMask(QCoreApplication::translate("one_class", "9999999999999999999", nullptr));
        lineEdit->setText(QString());
        label_2->setText(QCoreApplication::translate("one_class", "\345\215\241\345\217\267\357\274\232", nullptr));
        pushButton->setText(QCoreApplication::translate("one_class", "\347\231\273\345\275\225", nullptr));
        label_4->setText(QCoreApplication::translate("one_class", "\345\257\206\347\240\201\357\274\232", nullptr));
        lineEdit_2->setInputMask(QString());
        pushButton_2->setText(QCoreApplication::translate("one_class", "\345\217\226\346\266\210", nullptr));
        label_3->setText(QCoreApplication::translate("one_class", "YSU\351\223\266\350\241\214 \347\224\250\346\210\267\347\231\273\345\275\225", nullptr));
        register_2->setText(QCoreApplication::translate("one_class", "\346\263\250\345\206\214", nullptr));
        writeoff->setText(QCoreApplication::translate("one_class", "\346\263\250\351\224\200", nullptr));
        checkBox->setText(QCoreApplication::translate("one_class", "\346\230\276\347\244\272\345\257\206\347\240\201", nullptr));
    } // retranslateUi

};

namespace Ui {
    class one_class: public Ui_one_class {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ONE_CLASS_H
