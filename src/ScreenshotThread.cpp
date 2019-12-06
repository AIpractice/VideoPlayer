#include "ScreenshotThread.h"
#include <QDebug>
#include "Anime4KPlugin.h"
ScreenshotThread::ScreenshotThread(QObject* parent):QThread(parent){
    cnt = 0;
    videoName = "none";
}
ScreenshotThread::~ScreenshotThread(){

}

void ScreenshotThread::setVideoName(QString name){
    videoName = name;
    qDebug()<<videoName;
}

void ScreenshotThread::run(){
    qDebug()<<"screenshot saving...";
    cnt++;
    Anime4KPlugin::mutex.lock();
    Anime4KPlugin::Waifu2xFilter();
    Anime4KPlugin::currentFrame.save(QString("%1_%2_%3.png").arg(videoName).arg("screenshot").arg(cnt),"png");
    Anime4KPlugin::mutex.unlock();
    qDebug()<<"screenshot saved";
    emit done();
}
