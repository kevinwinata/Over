#ifndef TRACING_H
#define TRACING_H

#include <opencv2/core/core.hpp>

void segmentEdges(std::vector<std::vector<bool>>& contour, std::vector<std::vector<cv::Point>> edgemaps);
void separateEdges(std::vector<std::vector<bool>>& contour, std::vector<std::vector<cv::Point>>& edges, int rows, int cols);
void drawEdges(cv::Mat& img_edge, std::vector<std::vector<cv::Point>>& edgepoints);
void trace(std::vector<cv::Point> edgemap);

#endif