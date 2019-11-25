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
};

#endif // ANIME4KPLUGIN_H
