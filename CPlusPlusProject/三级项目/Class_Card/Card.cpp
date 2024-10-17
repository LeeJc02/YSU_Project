#include "Card.h"

void Card::GetCardNo() {
    cout << "用户名: " << this->user_name << endl;
    cout << "用户卡号: " << this->user_number << endl;
    cout << "用户密码: " << this->key_word << endl;
}

BankCard::BankCard(const BankCard &c) {
    balance = c.balance;
    user_name = c.user_name;
    user_number = c.user_number;
    key_word = c.key_word;
}

void BankCard::GetCardNo() {
    cout << "银行卡信息:" << endl;
    Card::GetCardNo(); // 调用基类的 displayInfo 函数
    cout << "余额: " << balance << endl;
//    qDebug() << QString::fromStdString(this->user_name);
}

double BankCard::balanceEnquiry() const {
    return this->balance;
}

bool BankCard::balanceWithdrawal(double amount) {
    if (amount > this->balance || amount < 0)
        return false;

    this->balance -= amount;
    return true;
}

bool BankCard::balanceRecharge(double rechargeAmount) {
    if (rechargeAmount <= 0)
        return false;

    this->balance += rechargeAmount;
    return true;
}

string BankCard::loanFunction() {
    return "暂无开通此功能";
}

bool BankCard::changePassword(const string &old_word, const string &new_word1, const string &new_word2) {
    for(const auto &i:old_word)
        if(!(i>='0' && i<='9' || i>='a' && i<='z' || i>='A' && i<='Z'))
            return false;
    for(const auto &i:new_word1)
        if(!(i>='0' && i<='9' || i>='a' && i<='z' || i>='A' && i<='Z'))
            return false;
    for(const auto &i:new_word2)
        if(!(i>='0' && i<='9' || i>='a' && i<='z' || i>='A' && i<='Z'))
            return false;
    if (old_word == this->key_word && new_word1 == new_word2) {
        this->key_word = new_word1;
        return true;
    }

    return false;
}

double BankCard::GetBalance() {
    return this->balance;
}

bool BankCard::isRoot() {
    qDebug() << "name:" << QString::fromStdString(this->user_name);
    if(this->user_name == "root")
        return true;
//    qDebug() << QString::fromStdString(user_number);
//    qDebug() << QString::fromStdString(key_word);
//    qDebug() << QString::fromStdString(this->user_name);
    return false;
}

string BankCard::lineData() {
    stringstream ss;
    ss << user_number << "," << key_word << "," << user_name << "," << to_string(balance);
    return ss.str();
}