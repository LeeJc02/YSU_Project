#include "ATM.h"

ATM::ATM(const string &path) {
    user_file_path = path;
    ifstream file(user_file_path);
    string line;
    // 逐行读取 CSV 文件
    while (getline(file, line)) {
        if(line.empty())    continue;
        istringstream iss(line);
        vector<string> tokens;
        string token;
        while (getline(iss, token, ','))
            tokens.push_back(token);
        pair<pair<string, string>, BankCard> temp = createCard(tokens);
//        BankCard temp_value = BankCard(tokens[0], tokens[1], tokens[2], stod(tokens[3]));
//        pair<string, string> temp_key = make_pair(tokens[0], tokens[1]);
        user_data.insert(temp);
    }
    user_data;
//    for (const auto &i: user_data)
//        qDebug() << QString::fromStdString(i.first.first) << " " << QString::fromStdString(i.first.second);
    file.close(); // 关闭文件
}

pair<pair<string, string>, BankCard> ATM::createCard(const vector<string> &line) {
    // 默认顺序为 卡号 - 密码 - 用户名 - 余额
//    string temp = line[0];
    Card temp_card = Card(line[0], line[1], line[2]);
    BankCard temp_value = BankCard(line[0], line[1], line[2], stod(line[3]));
    auto temp_key = pair<string, string>(line[0], line[1]);
//    qDebug() << QString::fromStdString(line[0]) << " " << QString::fromStdString(line[1]);
    return make_pair(temp_key, temp_value);
}

void ATM::userLogin(const string &username, const string &keyword, BankCard *&card) {
    for (const auto &i: user_data)
        qDebug() << QString::fromStdString(i.first.first) << " " << QString::fromStdString(i.first.second);
    pair<string, string> p = make_pair(username, keyword);
//    qDebug() << "this:" << QString::fromStdString(username) << " " << QString::fromStdString(keyword);
    auto it = user_data.find(p);
    if (it != user_data.end())
        card = &(it->second);
    else card = nullptr;
//    qDebug() << QString::fromStdString(card->GetCardNo());
//    card->GetCardNo();

}

void ATM::userLogout(const std::string &username, const std::string &keyword) {
    user_data.erase(make_pair(username, keyword));
}

void ATM::userRegister(const string &username, const string &keyword, const BankCard &card) {
    user_data.insert(make_pair(make_pair(username, keyword), card));
}

void ATM::userExit() {
    ofstream file(user_file_path);
    if(!file.is_open()) return;
    for (auto& row : user_data)
        file << row.second.lineData() << endl;
    file.close();
}