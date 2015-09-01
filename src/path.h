#ifndef PATH_H
#define PATH_H

#include <opencv2/core/core.hpp>

class Path
{
public:
	std::vector<cv::Point> corners;
	cv::Point control1;
	cv::Point control2;
	bool isCurve;
	int max_x;
	int min_x;
	int max_y;
	int min_y;

	Path();

	void addCorner(cv::Point p);

	void bezierFit();

	void printProps();
};

#endif