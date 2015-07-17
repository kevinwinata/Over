#ifndef SEGMENTATION_H
#define SEGMENTATION_H

#include <opencv2/core/core.hpp>
#include <map>
#include "region.h"

void colorMapSegmentation(cv::Mat& img, std::vector<std::vector<int>>& labels, std::vector<Region>& props, int maxDistance);

void drawSegments(cv::Mat& img_seg, std::vector<std::vector<int>>& labels);

#endif