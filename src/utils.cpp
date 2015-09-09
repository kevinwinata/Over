#include "utils.h"
#include <iostream>

bool legalPoint(int i, int j, int rows, int cols)
{
	return (i >= 0 && i < rows && j >= 0 && j < cols);
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

bool isLeft(cv::Point l1, cv::Point l2, cv::Point p)
{
	return ((l2.x - l1.x)*(p.y - l1.y) - (l2.y - l1.y)*(p.x - l1.x)) > 0;
}

bool isIntersect(cv::Point p1, cv::Point p2, std::vector<std::vector<long>>& labels, long label)
{
	int x1 = p1.x; int y1 = p1.y;
	int x2 = p2.x; int y2 = p2.y;

	int delta_x(x2 - x1);
	signed char const ix((delta_x > 0) - (delta_x < 0));
	delta_x = std::abs(delta_x) << 1;

	int delta_y(y2 - y1);
	signed char const iy((delta_y > 0) - (delta_y < 0));
	delta_y = std::abs(delta_y) << 1;

	//if (labels[y1][x1] - 1 == label) return true;
	//std::cout << labels[y1][x1] << " ";

	if (delta_x >= delta_y) {
		int error(delta_y - (delta_x >> 1));

		while (x1 != x2) {
			if ((error >= 0) && (error || (ix > 0))) {
				error -= delta_x;
				y1 += iy;
			}
			error += delta_y;
			x1 += ix;

			if (x1 != x2 && labels[y1][x1] - 1 == label) return true;
			//std::cout << labels[y1][x1] << " ";
		}
	}
	else {
		int error(delta_x - (delta_y >> 1));

		while (y1 != y2)
		{
			if ((error >= 0) && (error || (iy > 0))) {
				error -= delta_y;
				x1 += ix;
			}
			error += delta_x;
			y1 += iy;

			if (y1 != y2 && labels[y1][x1] - 1 == label) return true;
			//std::cout << labels[y1][x1] << " ";
		}
	}
	return false;
}

void simplify(std::vector<Region>& regions, long parent, long child, std::vector<Path>& paths, std::vector<std::vector<long>>& labels)
{
	std::vector<int> commonedge;
	for (int e1 : regions[parent].edges) {
		for (int e2 : regions[child].edges) {
			if (e1 == e2) commonedge.push_back(e1);
		}
	}

	for (int e : commonedge) {
		int curidx = 0;
		int lastidx = (int)paths[e].corners.size();

		while (curidx < lastidx) {
			cv::Point& curpoint = paths[e].corners[curidx];
			int legalidx = curidx + 1;

			for (int i = curidx + 2; i < lastidx; i++) {
				cv::Point& targetpoint = paths[e].corners[i];

				if (!isIntersect(curpoint, targetpoint, labels, parent)) {
					legalidx = i;
				}
			}

			int deleteidx = curidx + 1;
			while (deleteidx < legalidx) {
				regions[parent].deletelist.push_back(paths[e].corners[deleteidx]);
				//std::cout << "delete " << paths[e].corners[deleteidx] << "\n";
				deleteidx++;
			}

			curidx = legalidx;
		}
	}
}