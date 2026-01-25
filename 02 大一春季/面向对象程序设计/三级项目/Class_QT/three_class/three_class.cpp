//
// Created by Lee on 2024/4/18.
//

// You may need to build the project (run Qt uic code generator) to get "ui_three_class.h" resolved

#include "three_class.hpp"
#include "ui_three_class.h"


three_class::three_class(ATM &a, QWidget *parent) :
        QWidget(parent), ui(new Ui::three_class), atm(&a) {
    ui->setupUi(this);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    DOS redirect(ui->textEdit);
    buf = cout.rdbuf(&redirect);
    ui->textEdit->installEventFilter(this);
    connect(ui->cancel, &QPushButton::clicked, this, &three_class::saveQuit);
    connect(this, &three_class::enterPressed, this, &three_class::executeFunction);
//    atm = new ATM("D:\\Using\\CLion\\CLionProject\\CPP_Project6.0\\user_data.csv");
    displayMessage("Please enter \"start\" to start the program\n $ > ");
    if (ui->textEdit)
        oldText = ui->textEdit->toPlainText();

    currentState = State::Start;
}

void three_class::displayMessage(const QString &s) {
    ui->textEdit->append(s);
}

void three_class::saveQuit() {
    atm->userExit();
    this->close();
}

void three_class::executeFunction(const QString &input) {
    if (input.isEmpty()) return;
    switch (currentState) {
        case State::Start: {
            user_number.clear();
            key_word.clear();
            card = nullptr;
            displayMessage("Please enter the card number:\n $ > ");
            oldText = ui->textEdit->toPlainText();
            currentState = State::EnterCardNumber;
            break;
        }

        case State::EnterCardNumber:
            user_number = input;
            displayMessage("Please enter the card PIN:\n $ > ");
            oldText = ui->textEdit->toPlainText();
            currentState = State::EnterPin;
            break;
        case State::EnterPin:
            key_word = input;
            atm->userLogin(user_number.toStdString(), key_word.toStdString(), card);
            if (!card) {
                displayMessage("Login failed Please login again\n $ > ");
                oldText = ui->textEdit->toPlainText();
                currentState = State::Start;
                break;
            }
            displayMessage("Successful login\n");
            displayMessage(
                    "Please select a transaction item:\n1. Query 2. Withdraw 3. Recharge 4. Change password 5. Exit\n $ > ");
            oldText = ui->textEdit->toPlainText();
            currentState = State::Transaction;
            break;
        case State::Transaction:
            switch (input.toInt()) {
                case 1:
                    displayMessage(QString::number(card->GetBalance())+"\n");
                    displayMessage(
                            "Please select a transaction item:\n1. Query 2. Withdraw 3. Recharge 4. Change password 5. Exit\n $ > ");
                    oldText = ui->textEdit->toPlainText();
                    break;
                case 2:
                    displayMessage("Please enter the withdrawal amount\n $ > ");
                    oldText = ui->textEdit->toPlainText();
                    currentState = State::Withdraw;
                    break;
                case 3:
                    displayMessage("Please enter the recharge amount\n $ > ");
                    oldText = ui->textEdit->toPlainText();
                    currentState = State::Recharge;
                    break;
                case 4:
                    displayMessage("Please enter your old password\n $ > ");
                    oldText = ui->textEdit->toPlainText();
                    currentState = State::ChangeOld;
                    break;
                case 5:
//                    atm->userExit();
                    displayMessage("Successfully exit and save\n $ > ");
                    currentState = State::Start;
            }
            break;
        case State::Withdraw: {
            double need = input.toDouble();
            if (!card->balanceWithdrawal(need))
                displayMessage("Insufficient balance or error\n");
            else displayMessage("Successful withdrawal\n");
            displayMessage(
                    "Please select a transaction item:\n1. Query 2. Withdraw 3. Recharge 4. Change password 5. Exit\n $ > ");
            oldText = ui->textEdit->toPlainText();
            currentState = State::Transaction;
            break;
        }
        case State::Recharge:
            card->balanceRecharge(input.toDouble());
            displayMessage("Successful recharge\n");
            displayMessage(
                    "Please select a transaction item:\n1. Query 2. Withdraw 3. Recharge 4. Change password 5. Exit\n $ > ");
            oldText = ui->textEdit->toPlainText();
            currentState = State::Transaction;
            break;
        case State::ChangeOld:
            old_key = input;
            displayMessage("Please enter your new password\n $ > ");
            oldText = ui->textEdit->toPlainText();
            currentState = State::ChangeNew;
            break;
        case State::ChangeNew:
            new_key = input;
            displayMessage("Please re-enter the new password\n $ > ");
            oldText = ui->textEdit->toPlainText();
            currentState = State::ChangeRe;
            break;
        case State::ChangeRe:
            re_key = input;
            bool check = isValidPassword(old_key.toStdString()) && isValidPassword(new_key.toStdString()) &&
                  isValidPassword(re_key.toStdString());
            check = check && card->changePassword(old_key.toStdString(), new_key.toStdString(), re_key.toStdString());
            if(!check)
                displayMessage("Failed to change the password\n");
            else displayMessage("Password changed successfully\n");
            displayMessage(
                    "Please select a transaction item:\n1. Query 2. Withdraw 3. Recharge 4. Change password 5. Exit\n $ > ");
            oldText = ui->textEdit->toPlainText();
            currentState = State::Transaction;
            break;
    }
}

bool three_class::eventFilter(QObject *obj, QEvent *event) {

    if (obj == ui->textEdit && event->type() == QEvent::KeyPress) {
        auto *keyEvent = dynamic_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            auto temp = ui->textEdit->toPlainText();
            if (oldText.isEmpty())
                newText = temp;
            else
                newText = findDifference(temp, oldText);
            oldText = temp;
            emit enterPressed(newText);
            return true;
        } else if (keyEvent->key() == Qt::Key_Backspace) {
            QTextCursor cursor = ui->textEdit->textCursor();
            if (cursor.positionInBlock() == 0) {
                // 如果光标在当前行的开头，阻止 Backspace 事件传递
                return true;
            }
        }
    }
    return QObject::eventFilter(obj, event);
}


QString three_class::findDifference(const QString &str1, const QString &str2) {
    return str1.mid(qMin(str1.length(), str2.length()));
}

three_class::~three_class() {
    cout.rdbuf(buf);
    delete ui;
    delete buf;
}

bool three_class::isValidPassword(const string &password) {//检测银行卡密码是否有效的函数
    // 检查密码长度是否为6
    if (password.length() != 6)
        return false;

    // 检查密码是否只包含字母或数字
    for(const auto &c: password)
        if (!isalnum(c))  // isalnum() 检查是否为字母或数字
            return false;
    return true;
}

