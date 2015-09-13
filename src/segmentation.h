#ifndef SEGMENTATION_H
#define SEGMENTATION_H

#include <opencv2/core/core.hpp>
#include "region.h"

void colorMapSegmentation(cv::Mat& img, std::vector<std::vector<long>>& labels, std::vector<Region>& regions, int maxDistance);

#endif