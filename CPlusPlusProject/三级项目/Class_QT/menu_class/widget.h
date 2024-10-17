#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "../enquiry_class//enquiry.h"
#include "../logan_class/logan.h"
#include "../recharge_class/recharge.h"
#include "../withdrawal_class/withdrawal.h"
#include "../change_class/change.h"
#include "../../Class_Card/Card.h"
#include <QDebug>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget {
Q_OBJECT

public:
    Widget(BankCard &c, QWidget *parent = nullptr);

    ~Widget();

private slots:

    void on_logan_clicked();

    void on_Enquiry_clicked();

    void on_Recharge_clicked();

    void on_Withdrawal_clicked();

    void on_change_clicked();

private:
    Ui::Widget *ui;
    Enquiry *wenquiry;
    logan *wlogan;
    recharge *wrecharge;
    withdrawal *wwithdrawal;
    change *wchange;
    BankCard *card;
};

#endif // WIDGET_H
