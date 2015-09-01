#ifndef TRACING_H
#define TRACING_H

#include <opencv2/core/core.hpp>
#include <array>
#include "region.h"
#include "path.h"

void segmentEdges(std::vector<std::vector<bool>>& contour, std::vector<std::vector<cv::Point>> edgemaps);
void separateEdges(std::vector<std::vector<bool>>& contour, std::vector<Path>& paths, std::vector<Region>& regions, std::vector<std::vector<long>>& labels, int rows, int cols);
void contourChainCode(std::vector<std::vector<char>>& contour, std::vector<std::vector<std::pair<cv::Point, int>>>& chains, std::vector<std::vector<long>>& labels, std::vector<Region>& regions, int rows, int cols);
void findCorner(std::vector<std::vector<std::pair<cv::Point, int>>>& chains, std::vector<Path>& paths, double threshold, int n);
void edgeSort(std::vector<Region>& regions, std::vector<Path>& paths);
void drawChains(cv::Mat& img_chain, std::vector<std::vector<std::pair<cv::Point, int>>>& chains);
void drawEdges(cv::Mat& img_edge, std::vector<std::vector<std::pair<cv::Point, int>>>& chains, std::vector<Path>& paths, int rows, int cols);
void drawCurves(cv::Mat& img_curve, std::vector<Path>& paths);
void writeEdgeVector(std::string filename, std::vector<Path>& paths, int width, int height);
void writeVector(std::string filename, std::vector<Region>& regions, std::vector<Path>& paths, int width, int height);
void trace(std::vector<Path>& paths);

#endif