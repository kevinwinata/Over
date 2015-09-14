#ifndef OUTPUT_H
#define OUTPUT_H

#include <opencv2/core/core.hpp>
#include <array>
#include "region.h"
#include "path.h"
#include "vectortree.h"

void drawSegments(cv::Mat& img_seg, std::vector<std::vector<long>>& labels);
void drawContour(cv::Mat& img_contour, std::vector<std::vector<char>>& edges);
void drawChains(cv::Mat& img_chain, std::vector<std::vector<std::pair<cv::Point, int>>>& chains);
void drawEdges(cv::Mat& img_edge, std::vector<std::vector<std::pair<cv::Point, int>>>& chains, std::vector<Path>& paths);
void drawCurves(cv::Mat& img_curve, std::vector<Path>& paths);
void writeEdgeVector(std::string filename, std::vector<Path>& paths, int width, int height);
void writeVector(std::string filename, std::vector<Region>& regions, std::vector<Path>& paths, int width, int height);
void writeOptimizedVector(std::string filename, std::list<int>& sortedregions, std::vector<Region>& regions, std::vector<Path>& paths, int width, int height);

#endif