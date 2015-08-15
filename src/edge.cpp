#include "edge.h"

Edge::Edge() {
	max_x = 0;
	min_x = INFINITY;
	max_y = 0;
	min_y = INFINITY;
};

void Edge::addPoints(cv::Point p) {
	points.push_back(p);
	if (max_x < p.x) max_x = p.x;
	if (min_x > p.x) min_x = p.x;
	if (max_y < p.y) max_y = p.y;
	if (min_y > p.y) min_y = p.y;
};