#ifndef VIDEOFRAME_H
#define VIDEOFRAME_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <memory>
#include <QImage>
#define VideoFramePtr std::shared_ptr<VideoFrame>

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavdevice/avdevice.h"
}


class VideoFrame
{
public:
    VideoFrame();
    ~VideoFrame();

    void initBuffer(const int &width, const int &height);

    void setYUVbuf(const uint8_t *buf);
    void setYbuf(const uint8_t *buf);
    void setUbuf(const uint8_t *buf);
    void setVbuf(const uint8_t *buf);

    uint8_t * buffer(){return mYuv420Buffer;}
    int width(){return mWidth;}
    int height(){return mHeight;}
    QImage toQImage();
    void fromQImage(QImage&);
protected:
    uint8_t *mYuv420Buffer;

    int mWidth;
    int mHeight;


    AVFrame *pFrame ;
    AVFrame *pFrameRGB;
    uint8_t * rgbBuffer;
    SwsContext *img_convert_ctx;
    SwsContext *img_restore_ctx;
};

#endif // VIDEOFRAME_H
