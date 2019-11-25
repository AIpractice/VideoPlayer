#include "VideoFrame.h"

VideoFrame::VideoFrame()
{
    mYuv420Buffer = nullptr;
    pFrame = nullptr;
    pFrameRGB = nullptr;
    rgbBuffer = nullptr;
    img_convert_ctx = nullptr;
    img_restore_ctx = nullptr;
}

VideoFrame::~VideoFrame()
{
    if (mYuv420Buffer != nullptr)
    {
        free(mYuv420Buffer);
        mYuv420Buffer = nullptr;
    }
    if(img_convert_ctx != nullptr)
    {
        sws_freeContext(img_convert_ctx);
        img_convert_ctx = nullptr;
    }
    if(img_restore_ctx != nullptr)
    {
        sws_freeContext(img_restore_ctx);
        img_restore_ctx = nullptr;
    }
    if (rgbBuffer!=nullptr)
    {
        free(rgbBuffer);
        rgbBuffer = nullptr;
    }
    if(pFrame!=nullptr)
    {
        av_frame_free(&pFrame);
        pFrame = nullptr;
    }
    if(pFrameRGB!=nullptr)
    {
        av_frame_free(&pFrameRGB);
        pFrameRGB = nullptr;
    }
}

void VideoFrame::initBuffer(const int &width, const int &height)
{
    if (mYuv420Buffer != nullptr)
    {
        free(mYuv420Buffer);
        mYuv420Buffer = nullptr;
    }
    if(img_convert_ctx != nullptr)
    {
        sws_freeContext(img_convert_ctx);
        img_convert_ctx = nullptr;
    }
    if(img_restore_ctx != nullptr)
    {
        sws_freeContext(img_restore_ctx);
        img_restore_ctx = nullptr;
    }
    if (rgbBuffer!=nullptr)
    {
        free(rgbBuffer);
        rgbBuffer = nullptr;
    }
    if(pFrame!=nullptr)
    {
        av_frame_free(&pFrame);
        pFrame = nullptr;
    }
    if(pFrameRGB!=nullptr)
    {
        av_frame_free(&pFrameRGB);
        pFrameRGB = nullptr;
    }
    mWidth  = width;
    mHeight = height;

    mYuv420Buffer = (uint8_t*)malloc(width * height * 3 / 2);


    pFrame = av_frame_alloc();
    pFrameRGB = av_frame_alloc();
    int numBytes = avpicture_get_size(AV_PIX_FMT_RGB32,width,height);
    rgbBuffer = (uint8_t *) malloc(numBytes*sizeof(uint8_t));
    avpicture_fill((AVPicture *) pFrameRGB,rgbBuffer,AV_PIX_FMT_RGB32,width,height);
    img_convert_ctx = sws_getContext(width, height, AV_PIX_FMT_YUV420P, width, height, AV_PIX_FMT_RGB32, SWS_BICUBIC, NULL, NULL, NULL);
    img_restore_ctx = sws_getContext(width, height, AV_PIX_FMT_RGB32, width, height, AV_PIX_FMT_YUV420P,   SWS_BICUBIC, NULL, NULL, NULL);
}

void VideoFrame::setYUVbuf(const uint8_t *buf)
{
    int Ysize = mWidth * mHeight;
    memcpy(mYuv420Buffer, buf, Ysize * 3 / 2);
}

void VideoFrame::setYbuf(const uint8_t *buf)
{
    int Ysize = mWidth * mHeight;
    memcpy(mYuv420Buffer, buf, Ysize);
}

void VideoFrame::setUbuf(const uint8_t *buf)
{
    int Ysize = mWidth * mHeight;
    memcpy(mYuv420Buffer + Ysize, buf, Ysize / 4);
}

void VideoFrame::setVbuf(const uint8_t *buf)
{
    int Ysize = mWidth * mHeight;
    memcpy(mYuv420Buffer + Ysize + Ysize / 4, buf, Ysize / 4);
}

QImage VideoFrame::toQImage()
{
    avpicture_fill((AVPicture *) pFrame, (uint8_t *)mYuv420Buffer, AV_PIX_FMT_YUV420P, mWidth, mHeight);//这里的长度和高度跟之前保持一致
    //转换图像格式，将解压出来的YUV420P的图像转换为RGB的图像
    sws_scale(img_convert_ctx,(uint8_t const * const *) pFrame->data,pFrame->linesize, 0, mHeight, pFrameRGB->data,pFrameRGB->linesize);
    //把这个RGB数据 用QImage加载
    QImage tmpImg((uchar *)rgbBuffer,mWidth,mHeight,QImage::Format_RGB32);
    return tmpImg.copy();
}

void VideoFrame::fromQImage(QImage & qImage)
{
    assert(mWidth == qImage.width());
    assert(mHeight == qImage.height());
    pFrameRGB->data[0]=qImage.bits();
    sws_scale(img_restore_ctx,(uint8_t const* const*)pFrameRGB->data,pFrameRGB->linesize,0,mHeight,pFrame->data,pFrame->linesize);
}
