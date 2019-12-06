#ifndef SCREENSHOTTHREAD_H
#define SCREENSHOTTHREAD_H

#include <QThread>
#include <QImage>
#include "VideoPlayer/Video/VideoFrame.h"
class ScreenshotThread : public QThread
{
    Q_OBJECT
public:
    explicit ScreenshotThread(QObject *parent = 0);
    ~ScreenshotThread();

protected:
    //QThread的虚函数
    //线程处理函数
    //不能直接调用，通过start()间接调用
    void run();

signals:
    void done();  //处理完成信号

signals:

public slots:
};
#endif // SCREENSHOTTHREAD_H
