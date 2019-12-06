#include "ScreenshotThread.h"
#include <QDebug>
#include "Anime4KPlugin.h"
ScreenshotThread::ScreenshotThread(QObject* parent):QThread(parent){

}
ScreenshotThread::~ScreenshotThread(){

}


void ScreenshotThread::run(){
    qDebug()<<"screenshot saving...";
    Anime4KPlugin::mutex.lock();
    Anime4KPlugin::Waifu2xFilter();
    Anime4KPlugin::currentFrame.save("screenshot.png","png");
    Anime4KPlugin::mutex.unlock();
    qDebug()<<"screenshot saved";
    emit done();
}
