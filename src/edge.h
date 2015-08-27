#ifndef EDGE_H
#define EDGE_H

#include <opencv2/core/core.hpp>

class Edge
{
public:
	std::vector<cv::Point> corners;
	cv::Point control1;
	cv::Point control2;
	bool isCurve;
	bool reverse;
	int max_x;
	int min_x;
	int max_y;
	int min_y;

	Edge();

	void addCorner(cv::Point p);

	void bezierFit();

	void printProps();
};

#endif