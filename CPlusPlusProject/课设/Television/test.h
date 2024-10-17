#ifndef TEST_H
#define TEST_H

#include <QWidget>
#include <QtMultimedia/QtMultimedia>    // 视频播放器
#include <QtMultimediaWidgets/QVideoWidget> // 视频播放窗口
#include <QFileDialog>  // 文本选择器
#include <QMediaMetaData>   // 播放器数据处理
#include <QLabel>   // 文本编辑器
#include <QKeyEvent>    // 键响应事件
#include <QMouseEvent>  // 鼠标响应事件
#include <QSlider>  // 进度条
#include <QTimer>   // 定时器
#include <QMediaPlaylist>   // 播放列表
#include <QFileInfoList>    // 处理文件路径
#include <QDebug>
#include <QNetworkAccessManager>

namespace Ui {
class test;
}

class test : public QWidget
{
    Q_OBJECT

public:
    explicit test(QWidget *parent = nullptr);
    ~test();

private slots:
    void keyPressEvent(QKeyEvent *event);   // 键捕获

    void mousePressEvent(QMouseEvent *event);   // 鼠标捕获

    void on_horizontalSlider_valueChanged(int value); // 进度条

    void on_up_clicked();   // 音量增加

    void on_down_clicked(); // 音量减小

    void on_right_clicked();    // 进度快进

    void on_left_clicked();     // 进度后退

    void on_previous_clicked(); // 上一个

    void on_next_clicked();     // 下一个

    void on_center_clicked();   // 暂停/播放

    void updateSliderValue();   // 更新进度条

    void on_checkBox_stateChanged(int arg1);    // 更改checkbox状态

    void update_1(qint64 duration);    // 切换视频更新视频长度

    void update_2();    // 每秒更新播放label

    void on_sure_clicked();


private:
    void on_pushButton_clicked();   // 播放资源选择

    void on_pushButton_2_clicked(); // 播放

    void on_pushButton_3_clicked(); // 暂停

    void setPlayer();   // 初始化player

    Ui::test *ui;

    QMediaPlayer *player;   // 播放器

    QMediaPlaylist *playlist; // 播放列表

    QTimer *timer;          // 计时器

    QTimer *countdown;      // 倒数计时器

    QMediaContent *mediaContent; // 搜索媒体内容

    QNetworkAccessManager networkAccessManager;

    QNetworkReply *reply;
};

#endif // TEST_H
