#include "edge.h"

Edge::Edge() {
	max_x = 0;
	min_x = INFINITY;
	max_y = 0;
	min_y = INFINITY;
	isCurve = false;
	reverse = false;
};

void Edge::addCorner(cv::Point p) {
	corners.push_back(p);
	if (max_x < p.x) max_x = p.x;
	if (min_x > p.x) min_x = p.x;
	if (max_y < p.y) max_y = p.y;
	if (min_y > p.y) min_y = p.y;
};

void Edge::bezierFit() {
	double a1 = 0.0, a2 = 0.0, a12 = 0.0;
	cv::Point c1 = cv::Point(0.0, 0.0);
	cv::Point c2 = cv::Point(0.0, 0.0);

	int n = corners.size();
	for (int i = 1; i <= n; i++) {
		double ti = (double)(i - 1) / (n - 1);
		double qi = 1 - ti;
		double ti2 = ti * ti;
		double qi2 = qi * qi;
		double ti3 = ti * ti2;
		double qi3 = qi * qi2;
		double ti4 = ti * ti3;
		double qi4 = qi * qi3;
		a1 += ti2 * qi4;
		a2 += ti4 * qi2;
		a12 += ti3 * qi3;

		cv::Point pi = corners[i - 1];
		cv::Point m = pi - qi3 * corners.front() - ti3 * corners.back();
		c1 += ti * qi2 * m;
		c2 += ti2 * qi * m;
	}

	a1 *= 9.0;
	a2 *= 9.0;
	a12 *= 9.0;
	c1 *= 3.0;
	c2 *= 3.0;

	double d = a1 * a2 - a12 * a12;
	control1 = (a2 * c1 - a12 * c2) / d;
	control2 = (a1 * c2 - a12 * c1) / d;
}