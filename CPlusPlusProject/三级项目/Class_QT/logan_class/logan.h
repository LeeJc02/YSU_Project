#ifndef LOGAN_H
#define LOGAN_H

#include <QWidget>


namespace Ui {
class logan;
}

class logan : public QWidget
{
    Q_OBJECT

public:
    explicit logan(QWidget *parent = nullptr);
    ~logan();

private slots:
    void on_pushButton_clicked();

private:
    Ui::logan *ui;

};

#endif // LOGAN_H
