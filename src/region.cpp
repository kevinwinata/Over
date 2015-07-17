#include "region.h"
#include <iostream>

Region::Region() {
	r_sums = 0;
	g_sums = 0;
	b_sums = 0;
	n = 0;
}

void Region::addPixel(cv::Point3_<uchar>& pixel) {
	r_sums += pixel.z;
	g_sums += pixel.y;
	b_sums += pixel.x;
	n++;
}

std::string Region::getAvgColor() {
	int r = static_cast<int>(std::round(r_sums / n));
	int g = static_cast<int>(std::round(g_sums / n));
	int b = static_cast<int>(std::round(b_sums / n));

	std::stringstream s;
	s << "#" << std::hex << r << g << b;
	return s.str();
}

cv::Point3_<double> rgbToXyz(cv::Point3_<uchar> pixel) {
	double r = (double)pixel.z / 255.0;
	double g = (double)pixel.y / 255.0;
	double b = (double)pixel.x / 255.0;

	r = (r > 0.04045) ? pow((r + 0.055) / 1.055, 2.4) * 100 : (r / .1292);
	g = (g > 0.04045) ? pow((g + 0.055) / 1.055, 2.4) * 100 : (g / .1292);
	b = (b > 0.04045) ? pow((b + 0.055) / 1.055, 2.4) * 100 : (b / .1292);

	cv::Point3_<double> xyz;
	xyz.x = r * 0.4124 + g * 0.3576 + b * 0.1805;
	xyz.y = r * 0.2126 + g * 0.7152 + b * 0.0722;
	xyz.z = r * 0.0193 + g * 0.1192 + b * 0.9505;
	return xyz;
}

cv::Point3_<double> xyzToLab(cv::Point3_<double> xyz) {
	double x = xyz.x / 95.047;
	double y = xyz.y / 100.000;
	double z = xyz.z / 108.883;

	x = (x > 0.008856) ? pow(x, .3333333333) : ((7.787 * x) + (16.0 / 116.0));
	y = (y > 0.008856) ? pow(y, .3333333333) : ((7.787 * y) + (16.0 / 116.0));
	z = (z > 0.008856) ? pow(z, .3333333333) : ((7.787 * z) + (16.0 / 116.0));

	cv::Point3_<double> lab;
	lab.x = (116.0 * y) - 16.0;
	lab.y = 500.0 * (x - y);
	lab.z = 200.0 * (y - z);
	return lab;
}

double deltaE76(cv::Point3_<double> lab1, cv::Point3_<double> lab2) {
	return std::sqrt((lab1.x * lab2.x) + (lab1.y * lab2.y) + (lab1.z * lab2.z));
}

int Region::countDistToAvg(cv::Point3_<uchar>& pixel) {
	return	std::abs(r_sums / n - pixel.z) +
		std::abs(g_sums / n - pixel.y) +
		std::abs(b_sums / n - pixel.x);
}

double Region::countDiffToAvg(cv::Point3_<uchar>& pixel) {
	cv::Point3_<uchar> avg;
	avg.z = static_cast<int>(std::abs(r_sums / n - pixel.z));
	avg.y = static_cast<int>(std::abs(g_sums / n - pixel.y));
	avg.x = static_cast<int>(std::abs(b_sums / n - pixel.x));

	cv::Point3_<double> xyz1 = rgbToXyz(pixel);
	cv::Point3_<double> xyz2 = rgbToXyz(avg);

	return deltaE76(xyzToLab(xyz1), xyzToLab(xyz2));
}

void Region::printProps() {
	std::cout << "r_sums : " << r_sums << std::endl;
	std::cout << "g_sums : " << g_sums << std::endl;
	std::cout << "b_sums : " << b_sums << std::endl;
	std::cout << "n : " << n << std::endl;
	std::cout << "color : " << getAvgColor() << std::endl;
}