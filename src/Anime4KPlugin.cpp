#include "Anime4KPlugin.h"
#include <QImage>

Anime4KPlugin::Anime4KPlugin()
{
}

VideoFramePtr Anime4KPlugin::Anime4KFilter(VideoFramePtr vfp)
{
    QImage tmpImage = vfp.get()->toQImage();
    vfp.get()->fromQImage(tmpImage);
    return vfp;
}

