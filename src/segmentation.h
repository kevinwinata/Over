#ifndef SEGMENTATION_H
#define SEGMENTATION_H

#include <opencv2/core/core.hpp>
#include <map>
#include "region.h"

void colorMapSegmentation(cv::Mat& img, std::vector<std::vector<int>>& labels, std::vector<Region>& props, int maxDistance);

void findEdges(std::vector<std::vector<int>>& labels, std::vector<std::vector<int>>& edges, int rows, int cols);

void drawSegments(cv::Mat& img_seg, std::vector<std::vector<int>>& labels);

void drawEdges(cv::Mat& img_edge, std::vector<std::vector<int>>& edges);

#endif