#include "test.h"
#include "ui_test.h"


test::test(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::test)
{
//    QNetworkProxy proxy;
//    proxy.setType(QNetworkProxy::HttpProxy);
//    proxy.setHostName("127.0.0.1");
//    proxy.setPort(7890);
//    QNetworkProxy::setApplicationProxy(proxy);

    ui->setupUi(this);
    setPlayer();
    ui->widget->setFocus();
    ui->note->hide();

    // 在构造函数中启动定时器
    timer = new QTimer(this);
    countdown = new QTimer(this);

    connect(timer, &QTimer::timeout, this, &test::updateSliderValue);
    connect(countdown, &QTimer::timeout, this, &test::update_2);
    connect(player, &QMediaPlayer::durationChanged, this, &test::update_1);
    connect(player, &QMediaPlayer::positionChanged, this, &test::update_2);

    timer->start(200); // 每隔一秒更新一次
    ui->url->setPlaceholderText("请输入URL");

}

void test::setPlayer()
{
    player = new QMediaPlayer(this);
    player->setVideoOutput(ui->widget);
    playlist = new QMediaPlaylist();

    QDir directory("D:\\Using\\Qt\\QtProject\\Television\\VideoFiles");
    QFileInfoList fileInfoList = directory.entryInfoList(QDir::Files);

    // 将所有视频文件添加到播放列表中
    foreach (const QFileInfo &fileInfo, fileInfoList)
        if (fileInfo.suffix().toLower() == "mp4" || fileInfo.suffix().toLower() == "avi")
            playlist->addMedia(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));

    player->setPlaylist(playlist);
    player->setVolume(50); // 设置初始音量为 50%
    playlist->setPlaybackMode(QMediaPlaylist::Loop);
}

void test::update_1(qint64 duration)
{
    // 将视频时长转换为显示格式（例如 01:23）
    QString durationText = QTime(0, 0).addMSecs(duration).toString("mm:ss");
    ui->playLabel->setText(durationText);
}

void test::update_2()
{
    // 更新倒计时标签，倒计时从总时长开始
    if (player->duration() > 0) {
        qint64 remainingTime = player->duration() - player->position();
        QString remainingTimeText = QString("播放：%1 s").arg(remainingTime / 1000); // 将毫秒转换为秒
        ui->playLabel->setText(remainingTimeText);
    }
}

void test::on_pushButton_clicked()
{
    QString initialPath = "D:\\Using\\Qt\\QtProject\\Television\\VideoFiles";
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Video File"), initialPath, tr("Video Files (*.mp4)"));
    if (!fileName.isEmpty())
        player->setMedia(QUrl::fromLocalFile(fileName));
}

void test::on_pushButton_2_clicked()
{
    player->play();
}

void test::on_pushButton_3_clicked()
{
    player->pause();
}

void test::keyPressEvent(QKeyEvent *event)
{
    if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_Left)
        ui->previous->click();  // ctrl + left 上一集

    else if(event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_Right)
        ui->next->click();  // ctrl + right 上一集

    else
        switch(event->key()) {
            case Qt::Key_Return:    // Tab/Enter进入选择播放资源
            case Qt::Key_Enter:
                on_pushButton_clicked();
                break;
            case Qt::Key_Space:     // Space暂停/播放
                if (player->state() == QMediaPlayer::PlayingState)
                    on_pushButton_3_clicked();
                else    on_pushButton_2_clicked();
                break;
            case Qt::Key_Up:
                ui->up->click();          // 执行向上的操作
                break;
            case Qt::Key_Down:
                ui->down->click();        // 执行向下的操作
                break;
            case Qt::Key_Left:
                ui->left->click();        // 执行向左的操作
                break;
            case Qt::Key_Right:
                ui->right->click();        // 执行向右的操作
                break;
            default:
                QWidget::keyPressEvent(event);
        }
}

void test::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        ui->center->click();
    else
        QWidget::mousePressEvent(event);
}

void test::on_horizontalSlider_valueChanged(int value)
{
    ui->horizontalSlider->setValue(value);
}

void test::updateSliderValue()
{
    // 获取当前播放位置和总时长
    qint64 position = player->position();
    qint64 duration = player->duration();

    // 计算进度条的值并更新
    int value = 0;
    if (duration > 0)
    {
        value = (position * ui->horizontalSlider->maximum()) / duration;
    }
    ui->horizontalSlider->setValue(value);

}

test::~test()
{
    delete ui;
    delete player;
    delete timer;
}

void test::on_right_clicked()
{
    qint64 currentPosition = player->position(); // 获取当前播放位置
    qint64 newPosition = currentPosition + 5000; // 快进5秒
    player->setPosition(newPosition); // 设置新的播放位置

    ui->widget->setFocus();
}

void test::on_left_clicked()
{
    qint64 currentPosition = player->position(); // 获取当前播放位置
    qint64 newPosition = currentPosition - 5000; // 后退5秒
    player->setPosition(newPosition); // 设置新的播放位置

    ui->widget->setFocus();
}

void test::on_up_clicked()
{
    int currentVolume = player->volume();
    player->setVolume(currentVolume + 5); // 增大10个音量单位

    ui->volumeLabel->setText(QString("音量：%1%").arg(currentVolume));
    ui->widget->setFocus();
}

void test::on_down_clicked()
{
    int currentVolume = player->volume();
    player->setVolume(currentVolume - 5); // 减小10个音量单位

    ui->volumeLabel->setText(QString("音量：%1%").arg(currentVolume));
    ui->widget->setFocus();
}

void test::on_previous_clicked()
{
    playlist->previous();

    ui->widget->setFocus();
}

void test::on_next_clicked()
{
    playlist->next();

    ui->widget->setFocus();
}

void test::on_center_clicked()
{
    if (player->state() == QMediaPlayer::PlayingState)
        on_pushButton_3_clicked();
    else    on_pushButton_2_clicked();

    ui->widget->setFocus();
}


void test::on_checkBox_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked)
        ui->note->show();
    else
        ui->note->hide();

    ui->widget->setFocus();
}

void test::on_sure_clicked()
{
    if(ui->url->text().isEmpty())
    {
        ui->widget->setFocus();
        return;
    }

    qDebug() << "start";
    QString text = ui->url->text();
    QUrl resourceUrl(text);
    reply = networkAccessManager.get(QNetworkRequest(resourceUrl));

    // 处理资源文件的下载
    connect(reply, &QNetworkReply::finished, [&]() {
        qDebug() << "play";

        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << "Error: " << reply->errorString();
            return;
        }

        // 将下载的资源文件保存到指定目录中
        QString savePath = "D:/Using/Qt/QtProject/Television/VideoFiles/local_video.mp4";
        QFile file(savePath);
        qDebug() << "nihao";
        if (file.open(QIODevice::WriteOnly)) {
            file.write(reply->readAll());
            file.close();

            // 播放本地资源文件
            player->setMedia(QUrl::fromLocalFile(savePath));
            player->play();
        }
        else
            qDebug() << "Error: Cannot write to file.";

        reply->deleteLater();
    });

    ui->widget->setFocus();
}
