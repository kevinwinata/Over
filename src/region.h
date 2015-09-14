#ifndef REGION_H
#define REGION_H

#include <opencv2/core/core.hpp>
#include "path.h"

class Region
{
public:
	long r_sums;
	long g_sums;
	long b_sums;
	long n;
	bool relevant;
	std::vector<int> edges;
	std::vector<int> reversed;
	std::vector<int> disconnected;
	std::vector<cv::Point> deletelist;

	Region();

	void addPixel(cv::Point3_<uchar>& pixel);
	void addEdge(int idx);

	std::string getAvgColor();
	int countDistToAvg(cv::Point3_<uchar>& pixel);
	double countDiffToAvg(cv::Point3_<uchar>& pixel);

	void printProps();

	void simplify(Region& child, std::vector<Path>& paths);
};

#endif
