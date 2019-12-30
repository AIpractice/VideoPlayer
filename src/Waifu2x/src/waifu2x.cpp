/*
 * main.cpp
 *   (ここにファイルの簡易説明を記入)
 *
 *  Created on: 2015/05/24
 *      Author: wlamigo
 *
 *   (ここにファイルの説明を記入)
 */

#include <opencv2/opencv.hpp>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cmath>
#include <QDebug>
#include "picojson.h"
#include "Yx_opencv_ImgChange.h"

#include "modelHandler.hpp"
#include "convertRoutine.hpp"

int waifu2x(QImage *qimage) {

    int nrlevel = 2;
    int NumberOfJobs = 4;
    std::string ModelPath = "../src/Waifu2x/models";



    // load image file
    cv::Mat image = Yx_opencv_ImgChange::QImage2cvMat(*qimage);
    cv::imwrite("Screen shot from waifu2x (before).jpg", image);


    image.convertTo(image, CV_32F, 1.0 / 255.0);
    cv::cvtColor(image, image, cv::COLOR_RGB2YUV);



    // set number of jobs for processing models
    w2xc::modelUtility::getInstance().setNumberOfJobs(NumberOfJobs);

    // ===== Noise Reduction Phase =====
    if (true) {
        std::string modelFileName(ModelPath);
        modelFileName = modelFileName + "/noise"
                + std::to_string(nrlevel) + "_model.json";
        std::vector<std::unique_ptr<w2xc::Model> > models;
        qDebug() << modelFileName.data() << '\n';

        if (!w2xc::modelUtility::generateModelFromJSON(modelFileName, models)) {
            std::exit(-1);
        }
        std::vector<cv::Mat> imageSplit;
        cv::Mat imageY;
        cv::split(image, imageSplit);
        imageSplit[0].copyTo(imageY);

        printf("before convert with models\n");
        w2xc::convertWithModels(imageY, imageSplit[0], models);

        printf("after convert with models\n");
        cv::merge(imageSplit, image);
        printf("after merge\n");

    } // noise reduction phase : end
    printf("noise reduction phase : end\n");
    // ===== scaling phase =====

    cv::cvtColor(image, image, cv::COLOR_YUV2RGB);
    image.convertTo(image, CV_8U, 255.0);

    (*qimage) = Yx_opencv_ImgChange::cvMat2QImage(image);

    std::cout << "process successfully done!" << std::endl;

    return 0;
}

