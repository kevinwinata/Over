#include "region.h"
#include "utils.h"
#include <iostream>
#include <algorithm>

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

void Region::addEdge(int idx) {
	if (std::find(edges.begin(), edges.end(), idx) == edges.end()) {
		edges.push_back(idx);
	}
}

std::string Region::getAvgColor() {
	int r = static_cast<int>(std::round(r_sums / n));
	int g = static_cast<int>(std::round(g_sums / n));
	int b = static_cast<int>(std::round(b_sums / n));

	std::stringstream s;
	s << "#" << std::hex 
		<< ((r < 16) ? "0" : "") << r 
		<< ((g < 16) ? "0" : "") << g 
		<< ((b < 16) ? "0" : "") << b;
	return s.str();
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
	/*std::cout << "r_sums : " << r_sums << std::endl;
	std::cout << "g_sums : " << g_sums << std::endl;
	std::cout << "b_sums : " << b_sums << std::endl;
	std::cout << "n : " << n << std::endl;
	std::cout << "color : " << getAvgColor() << std::endl;*/
	std::cout << "edges : ";
	for (int i : edges) {
		std::cout << i << " ";
	}
}