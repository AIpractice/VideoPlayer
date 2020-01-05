#include "Anime4KPlugin.h"
#include <QImage>
#include "Waifu2x/src/waifu2x.h"
#include "ImageKernel.hpp"

#include <iostream>
//#include <opencv2\opencv.hpp>
#include <Windows.h>
using namespace std;
//using namespace cv;

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
    //@param argv the command line arguments
    if (false) {
        ImageKernel kernel;

        float scale = 2.0f;
        scale = 2.0f;
        float pushStrength = scale / 6.0f;
        float pushGradStrength = scale / 2.0f;

        kernel.setPushStrength(pushStrength);
        kernel.setPushGradStrength(pushGradStrength);
        kernel.setBufferedImage(tmpImage);
        kernel.process();
        kernel.updateBufferedImage();
    }

    //tmpImage = tmpImage.copy(100,100,tmpImage.width(),tmpImage.height());
    //tmpImage.fill(0x00FF0000u);


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
    printf("waifu2x start!\n");
    waifu2x(&currentFrame);
    printf("waifu2x end!\n");
    Sleep(30);
}
