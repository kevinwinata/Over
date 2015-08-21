#ifndef TRACING_H
#define TRACING_H

#include <opencv2/core/core.hpp>
#include <array>
#include "region.h"
#include "edge.h"

void segmentEdges(std::vector<std::vector<bool>>& contour, std::vector<std::vector<cv::Point>> edgemaps);
void separateEdges(std::vector<std::vector<bool>>& contour, std::vector<Edge>& edges, std::vector<Region>& regions, std::vector<std::vector<long>>& labels, int rows, int cols);
void contourChainCode(std::vector<std::vector<char>>& contour, int rows, int cols);
void findCorner(std::vector<std::vector<std::pair<cv::Point, int>>> chains, double threshold, int n);
void drawEdges(cv::Mat& img_edge, std::vector<Edge>& edgepoints);
void drawCurves(cv::Mat& img_curve, std::vector<Edge>& edges);
void writeVector(std::string filename, std::vector<Edge>& edges, int width, int height);
void trace(std::vector<Edge>& edges);

#endif