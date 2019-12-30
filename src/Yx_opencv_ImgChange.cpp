#include "Yx_opencv_ImgChange.h"
#include <QMessageBox>


using namespace cv;
using namespace std;

QImage Yx_opencv_ImgChange::cvMat2QImage(const Mat& mat)    // Mat 改成 QImage
{
    if (mat.type() == CV_8UC1)					// 单通道
    {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        image.setColorCount(256);				// 灰度级数256
        for (int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }
        uchar *pSrc = mat.data;					// 复制mat数据
        for (int row = 0; row < mat.rows; row++)
        {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    }

    else if (mat.type() == CV_8UC3)				// 3通道
    {
        const uchar *pSrc = (const uchar*)mat.data;			// 复制像素
        QImage image(pSrc, mat.cols, mat.rows, (int)mat.step, QImage::Format_RGB888);	// R, G, B 对应 0,1,2
        return image.rgbSwapped();				// rgbSwapped是为了显示效果色彩好一些。
    }
    else if (mat.type() == CV_8UC4)
    {
        const uchar *pSrc = (const uchar*)mat.data;			// 复制像素
            // Create QImage with same dimensions as input Mat
        QImage image(pSrc,mat.cols, mat.rows, (int)mat.step, QImage::Format_ARGB32);		// B,G,R,A 对应 0,1,2,3
        return image.copy();
    }
    else
    {
        return QImage();
    }
}

Mat Yx_opencv_ImgChange::QImage2cvMat(QImage image)			// QImage改成Mat
{
    Mat mat;
    switch (image.format())
    {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
        mat = Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
        break;
    case QImage::Format_RGB888:
        mat = Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
        cv::cvtColor(mat, mat, CV_BGR2RGB);
        break;
    case QImage::Format_Indexed8:
        mat = Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
        break;
    }
    return mat;
}

QImage Yx_opencv_ImgChange::splitBGR(QImage src, int color)			// 提取RGB分量
{
    Mat srcImg, dstImg;
    srcImg = QImage2cvMat(src);
    if (srcImg.channels() == 1)
    {
        //QMessageBox message(QMessageBox::Information, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("该图像为灰度图像。"));
        //message.exec();
        return src;
    }
    else
    {
        vector<Mat> m;
        split(srcImg, m);
        vector<Mat>Rchannels, Gchannels, Bchannels;
        split(srcImg, Rchannels);
        split(srcImg, Gchannels);
        split(srcImg, Bchannels);
        Rchannels[1] = 0;	Rchannels[2] = 0;
        Gchannels[0] = 0;	Gchannels[2] = 0;
        Bchannels[0] = 0;	Bchannels[1] = 0;
        merge(Rchannels, m[0]);
        merge(Gchannels, m[1]);
        merge(Bchannels, m[2]);

        dstImg = m[color];		// 分别对应B、G、R
        QImage dst = cvMat2QImage(dstImg);
        return dst;
    }
}

