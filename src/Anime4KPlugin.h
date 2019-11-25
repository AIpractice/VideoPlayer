#ifndef ANIME4KPLUGIN_H
#define ANIME4KPLUGIN_H
#include "VideoPlayer/VideoPlayer.h"
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavdevice/avdevice.h"
}


class Anime4KPlugin
{
public:
    static VideoFramePtr Anime4KFilter(VideoFramePtr);
private:
    Anime4KPlugin();
    static uint8_t *yuvBuffer;
    static AVFrame *pFrame ;
    static AVFrame *pFrameRGB;
    static uint8_t * rgbBuffer;
    static SwsContext *img_convert_ctx;
};

#endif // ANIME4KPLUGIN_H
