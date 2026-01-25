#ifndef ENQUIRY_H
#define ENQUIRY_H

#include <QWidget>

namespace Ui {
class Enquiry;
}

class Enquiry : public QWidget
{
    Q_OBJECT

public:
    explicit Enquiry(double balance, QWidget *parent = nullptr);
    ~Enquiry();




private slots:
    void on_pushButton_clicked();

private:
    Ui::Enquiry *ui;
    QWidget *test;
};

#endif // ENQUIRY_H
