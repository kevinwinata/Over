#ifndef TRACING_H
#define TRACING_H

#include <opencv2/core/core.hpp>
#include <array>
#include "region.h"
#include "edge.h"

void segmentEdges(std::vector<std::vector<bool>>& contour, std::vector<std::vector<cv::Point>> edgemaps);
void separateEdges(std::vector<std::vector<bool>>& contour, std::vector<Edge>& edges, std::vector<Region>& regions, std::vector<std::vector<long>>& labels, int rows, int cols);
void drawEdges(cv::Mat& img_edge, std::vector<Edge>& edgepoints);
void drawCurves(cv::Mat& img_curve, std::vector<Edge>& edges);
void trace(std::vector<Edge>& edges);

#endif