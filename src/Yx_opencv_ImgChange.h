#include <QImage>
#include <opencv2/opencv.hpp>

class Yx_opencv_ImgChange;
// 图像转换
class Yx_opencv_ImgChange
{
public:
    Yx_opencv_ImgChange();
    ~Yx_opencv_ImgChange();
    static QImage cvMat2QImage(const cv::Mat& mat);     // Mat 改成 QImage
    static cv::Mat QImage2cvMat(QImage image);			// QImage 改成 Mat
    QImage splitBGR(QImage src, int color);			// 提取RGB分量
};
