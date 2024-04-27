//
// Created by Lee on 2024/4/18.
//

#include "DOS.h"

DOS::int_type DOS::overflow(int_type v) {
    if (v == '\n') {
        textEdit->append("");
    } else {
        textEdit->moveCursor(QTextCursor::End);
        textEdit->insertPlainText(QString(static_cast<char>(v)));
    }
    return v;
}