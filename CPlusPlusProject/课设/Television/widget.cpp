#include "widget.h"
#include "ui_widget.h"
#include <QtMultimedia/QtMultimedia>
#include <QtMultimediaWidgets/QVideoWidget>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    auto * videoWin = new QVideoWidget(this);
    videoWin->resize(400,300);

    auto * player = new QMediaPlayer(this);
    player->setVideoOutput(videoWin);
    player->setMedia(QMediaContent(QUrl::fromLocalFile("D:/Using/Qt/QtProject/untitled/Resource/matter.mp4")));
    player->play();

}

Widget::~Widget()
{
    delete ui;
}

