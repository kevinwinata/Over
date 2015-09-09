#ifndef UTILS_H
#define UTILS_H

#include <opencv2/core/core.hpp>
#include "region.h"

bool legalPoint(int i, int j, int rows, int cols);
cv::Point3_<double> rgbToXyz(cv::Point3_<uchar> pixel);
cv::Point3_<double> xyzToLab(cv::Point3_<double> xyz);
double deltaE76(cv::Point3_<double> lab1, cv::Point3_<double> lab2);
bool isLeft(cv::Point l1, cv::Point l2, cv::Point p);
bool isIntersect(cv::Point p1, cv::Point p2, std::vector<std::vector<long>>& labels, long label);
void simplify(std::vector<Region>& regions, long parent, long child, std::vector<Path>& paths, std::vector<std::vector<long>>& labels);

#endif