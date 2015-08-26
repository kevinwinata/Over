#ifndef SEGMENTATION_H
#define SEGMENTATION_H

#include <opencv2/core/core.hpp>
#include "region.h"

void colorMapSegmentation(cv::Mat& img, std::vector<std::vector<long>>& labels, std::vector<Region>& regions, int maxDistance);

void findContour(std::vector<std::vector<long>>& labels, std::vector<std::vector<char>>& contour, int rows, int cols);

void drawSegments(cv::Mat& img_seg, std::vector<std::vector<long>>& labels);

void drawContour(cv::Mat& img_contour, std::vector<std::vector<char>>& edges);

#endif