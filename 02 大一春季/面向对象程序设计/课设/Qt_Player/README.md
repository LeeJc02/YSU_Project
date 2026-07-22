# Qt_Player

使用QT + C++搭建的一个基础的本地视频播放器, 基于qmake构建, Qt Creator实现


---

# 窗体设计

- 主体以QWidget作为承载，另起一个QWidget提升为QVideoWidget作为播放器的载具
- 设计QMediaPlayer部件作为视频播放器，同时设计七个基础QPushButton键位用于实现基础的按键
- 设计一个QTextEdit部件作为快捷键提示文本，并用一个QCheckBox控件进行控制收放
- 设计一个QSlider作为视频播放进度条
- 设计好几个QLabel作为提示性文本的载具，如播放剩余事件和音量
- 同时提供一个QLineEdit用于输入URL路径和一个用于确认的QPushButton按键

# 快捷键绑定

所有部件通过 Qt 信号槽和键盘事件绑定。URL 输入框与确认按钮无快捷键。

# 功能实现

1. 一个窗体一个类，通过main.cpp文件调用窗口文件实现对窗口的打开调用
2. 大体上来说通过keyPressEvent捕获键盘事件，触发各个按键的信号或者单独实现的槽函数，实现快捷键绑定
3. 各个按键通过如clicked等信号与相对应的实现功能的槽函数相连接，实现按键响应
4. 关于音量的调节：通过获取QMediaPlayer的volume值再重新设置更改音量，同时修改音量的百分比显示
5. 关于进度的调节：通过获取QMediaPlayer的position值再重新设置更改进度，同时修改剩余播放时间的秒数和QSlider的进度条位置
6. 关于选集：设置存储播放资源的固定QString路径并使用QFileDialog窗口打开选择文件
7. 关于切换集：设置QMediaPlaylist，将固定路径内所有播放文件载入进去，以Loop形式循环播放形成链式
8. 关于暂停：捕获鼠标点击和键盘空格，通过检测QMedioPlayer当前状态来进行暂停/播放
9. 关于下载URL：通过QNetworkAccessManager的get请求获取URL视频资源并下载到本地固定视频目录内(有点麻烦放代码详解里)

# 代码详解

仓库不包含本地测试视频，请在运行时自行选择媒体文件。
