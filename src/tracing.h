#ifndef TRACING_H
#define TRACING_H

#include <opencv2/core/core.hpp>
#include <array>
#include "region.h"
#include "path.h"
#include "vectortree.h"

void findContour(std::vector<std::vector<long>>& labels, std::vector<std::vector<char>>& contour, int rows, int cols);
void contourChainCode(std::vector<std::vector<char>>& contour, std::vector<std::vector<std::pair<cv::Point, int>>>& chains, std::vector<std::vector<long>>& labels, std::vector<Region>& regions, VectorTree& tree, int rows, int cols);
void findCorner(std::vector<std::vector<std::pair<cv::Point, int>>>& chains, std::vector<Path>& paths, double threshold, int n);
void edgeSort(std::vector<Region>& regions, std::vector<Path>& paths, int t);

#endif