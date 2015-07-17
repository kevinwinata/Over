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

int Region::countDistToAvg(cv::Point3_<uchar>& pixel) {
	return	std::abs(r_sums / n - pixel.z) +
		std::abs(g_sums / n - pixel.y) +
		std::abs(b_sums / n - pixel.x);
}

void Region::printProps() {
	std::cout << "r_sums : " << r_sums << std::endl;
	std::cout << "g_sums : " << g_sums << std::endl;
	std::cout << "b_sums : " << b_sums << std::endl;
	std::cout << "n : " << n << std::endl;
	std::cout << "color : " << getAvgColor() << std::endl;
}