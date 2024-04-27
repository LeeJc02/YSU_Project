#ifndef MY_CARD_H
#define MY_CARD_H

#include <string>
#include <iostream>
#include <QDebug>
#include <QString>
#include <sstream>

using namespace std;

class Card {

protected:
    string user_number; // 用户卡号
    string key_word;    // 用户密码
    string user_name;   // 用户名

public:
    Card() = default; // 防止一些危险行为
    Card(string a, string b, string c) // 有参构造通过语义传参
            : user_number(a), key_word(b), user_name(c) {}
    Card(const Card &c) : user_number(c.user_number), key_word(c.key_word), user_name(c.user_name) {}
    virtual void GetCardNo(); // 虚函数
};

class BankCard : public Card { // public形式继承
public:
    double balance;     // 用户余额
    BankCard() = default;
    BankCard(string a,string b, string c, double d)
            : Card(a, b, c), balance(d) {}
    BankCard(Card c, double d) : Card(c), balance(d) {}
    BankCard(const BankCard &c);
    void GetCardNo() override;    // 覆写虚函数
    double balanceEnquiry() const;    // 余额查询, const修饰避免修改数据
    bool balanceWithdrawal(double amount); // 余额提取
    bool balanceRecharge(double rechargeAmount);   // 余额充值, static修饰表示无关类内数据
    static string loanFunction();       // 贷款功能(提示暂无开通此功能即可)
    bool changePassword(const string &old_word, const string &new_word1, const string &new_word2);      // 修改密码
    double GetBalance();
    bool isRoot();
    string lineData();
};

#endif