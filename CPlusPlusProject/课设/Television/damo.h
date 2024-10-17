#ifndef DAMO_H
#define DAMO_H

#include <QWidget>

namespace Ui {
class damo;
}

class damo : public QWidget
{
    Q_OBJECT

public:
    explicit damo(QWidget *parent = nullptr);
    ~damo();

private:
    Ui::damo *ui;
};

#endif // DAMO_H
