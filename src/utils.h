#ifndef UTILS_H
#define UTILS_H

#include <opencv2/core/core.hpp>

bool legalPoint(int i, int j, int rows, int cols);
cv::Point3_<double> rgbToXyz(cv::Point3_<uchar> pixel);
cv::Point3_<double> xyzToLab(cv::Point3_<double> xyz);
double deltaE76(cv::Point3_<double> lab1, cv::Point3_<double> lab2);

#endif