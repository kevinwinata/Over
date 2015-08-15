#ifndef EDGE_H
#define EDGE_H

#include <opencv2/core/core.hpp>

class Edge
{
public:
	int max_x;
	int min_x;
	int max_y;
	int min_y;
	std::vector<cv::Point> points;

	Edge();

	void addPoints(cv::Point p);

	void printProps();
};

#endif