//
// Created by Lee on 2024/4/21.
//

#ifndef CPP_PROJECT4_0_WARNING_H
#define CPP_PROJECT4_0_WARNING_H

#include <QWidget>
#include <QString>
#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class warning; }
QT_END_NAMESPACE

class warning : public QDialog {
Q_OBJECT

public:
    explicit warning(const QString &node, QWidget *parent = nullptr);

    ~warning() override;

private:
    Ui::warning *ui;
};


#endif //CPP_PROJECT4_0_WARNING_H
