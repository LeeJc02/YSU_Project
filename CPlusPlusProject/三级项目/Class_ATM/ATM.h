#ifndef MY_ATM_H
#define MY_ATM_H

#include <string>
#include <map>
#include "../Class_Card/Card.h"
#include <vector>
#include <fstream>
#include <sstream>
#include <QDebug>
#include <QString>

using namespace std;

class ATM { // 用于对各用户Card的调用和操作
    friend class csv;
public:
    explicit ATM(const string & user_file_path); // 有参构造, 将传入用户数据文件的路径进行读取并存储
    void userLogin(const string &username,const string &keyword, BankCard *&card);       // 用户登录
    void userExit();        // 用户退出, 退出时将修改后的数据更新数据文件
    void userLogout(const string &username, const string &keyword);      // 用户注销
    void userRegister(const string &username, const string &keyword, const BankCard &card);    // 用户注册

//    map<pair<string, string>, BankCard> user_data;  // 用于存储用户信息, 前者存储用户卡号和密码, 后者存储所有信息
protected:
    string user_file_path;
    static pair<pair<string, string>, BankCard> createCard(const vector<string> &line);
    map<pair<string, string>, BankCard> user_data;  // 用于存储用户信息, 前者存储用户卡号和密码, 后者存储所有信息
};


#endif