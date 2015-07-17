#ifndef REGION_H
#define REGION_H

#include <opencv2/core/core.hpp>

class Region
{
private:
	long r_sums;
	long g_sums;
	long b_sums;
	long n;

public:
	Region();

	void addPixel(cv::Point3_<uchar>& pixel);
	std::string getAvgColor();
	int countDistToAvg(cv::Point3_<uchar>& pixel);
	void printProps();
};

#endif
