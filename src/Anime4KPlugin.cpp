#include "Anime4KPlugin.h"
#include <QImage>

QImage Anime4KPlugin::currentFrame;
QMutex Anime4KPlugin::mutex;
Anime4KPlugin::Anime4KPlugin()
{
}

VideoFramePtr Anime4KPlugin::Anime4KFilter(VideoFramePtr vfp)
{
    QImage tmpImage = vfp.get()->toQImage();
    /*
     *
     * 此处应当对tmpImage 进行Anime4K算法处理
     *
     */
    vfp.get()->fromQImage(tmpImage);
    if(mutex.tryLock()){
        currentFrame = tmpImage;
        mutex.unlock();
    }
    return vfp;
}

void Anime4KPlugin::Waifu2xFilter(){
    /*
     *
     * 此处应当对currentFrame进行waifu2x+Anime4k处理
     *
     */
    Sleep(3000);
}
