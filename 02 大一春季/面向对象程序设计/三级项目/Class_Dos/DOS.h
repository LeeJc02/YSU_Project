//
// Created by Lee on 2024/4/18.
//

#ifndef CPP_PROJECT4_0_DOS_H
#define CPP_PROJECT4_0_DOS_H

#include <iostream>
#include <streambuf>
#include <QTextEdit>

using namespace std;

class DOS : public std::streambuf {
public:
    explicit DOS(QTextEdit* textEdit) : textEdit(textEdit) {}

protected:
    int_type overflow(int_type v) override ;

private:
    QTextEdit* textEdit;
};


#endif //CPP_PROJECT4_0_DOS_H
