#include "tracing.h"
#include "utils.h"
#include <array>
#include <iostream>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

void segmentEdges(std::vector<std::vector<bool>>& contour, std::vector<std::vector<cv::Point>>& edgepoints, int rows, int cols)
{
	std::array<std::pair<int, int>, 8> dir;
	dir[0] = std::make_pair(0, 1);
	dir[1] = std::make_pair(-1, 1);
	dir[2] = std::make_pair(-1, 0);
	dir[3] = std::make_pair(-1, -1);
	dir[4] = std::make_pair(0, -1);
	dir[5] = std::make_pair(1, -1);
	dir[6] = std::make_pair(1, 0);
	dir[7] = std::make_pair(1, 1);

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			if (contour[i][j]) {
				std::vector<cv::Point> edgepoint;
				cv::Point* p = &(cv::Point(j, i));

				while (p != NULL) {
					edgepoint.push_back(*p);
					contour[p->y][p->x] = false;

					int n;
					bool found = false;
					for (n = 0; n < dir.size(); n++) {
						int ypos = p->y + dir[n].first;
						int xpos = p->x + dir[n].second;
						if (legalPoint(ypos,xpos,rows,cols) && contour[ypos][xpos]) {
							p = &(cv::Point(xpos, ypos));
							found = true;
							break;
						}
					}

					if (found && (n - 1) % 2 == 0) {
						if (legalPoint(i + dir[n].first, j, rows, cols)) {
							contour[i + dir[n].first][j] = false;
						}
						if (legalPoint(i, j + dir[n].second, rows, cols)) {
							contour[i][j + dir[n].second] = false;
						}
					}

					if (!found) p = NULL;
				}

				edgepoints.push_back(edgepoint);
			}
		}
	}

	std::cout << "total edges : " << edgepoints.size() << "\n";
}

void separateEdges(std::vector<std::vector<bool>>& contour, std::vector<Edge>& edges, std::vector<Region>& regions, std::vector<std::vector<long>>& labels, int rows, int cols)
{
	std::array<std::pair<int, int>, 8> dir;
	dir[0] = std::make_pair(0, 1);
	dir[1] = std::make_pair(-1, 1);
	dir[2] = std::make_pair(-1, 0);
	dir[3] = std::make_pair(-1, -1);
	dir[4] = std::make_pair(0, -1);
	dir[5] = std::make_pair(1, -1);
	dir[6] = std::make_pair(1, 0);
	dir[7] = std::make_pair(1, 1);

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			if (contour[i][j]) {
				Edge edge;
				cv::Point* p = &(cv::Point(j, i));

				bool stop = false;
				int prevdir = 0;

				while (!stop) {
					edge.addPoints(*p);
					contour[p->y][p->x] = false;

					int n;
					bool found = false;

					regions[labels[p->y][p->x]-1].addEdge(edges.size());
					if (p->y > 0)
					regions[labels[p->y-1][p->x]-1].addEdge(edges.size());
					if (p->x > 0)
					regions[labels[p->y][p->x-1]-1].addEdge(edges.size());

					for (n = 7; n >= 0; n--) {
						int ypos = p->y + dir[n].first;
						int xpos = p->x + dir[n].second;
						if (legalPoint(ypos, xpos, rows, cols) && contour[ypos][xpos]) {
							p = &(cv::Point(xpos, ypos));
							found = true;
							break;
						}
					}

					if (!found || !dirDiff(prevdir, n)) {
						//if (prevdir > 3 && prevdir < 7)
							//std::cout << prevdir << " " << n << "\n";
						stop = true;
					}
					prevdir = n;
				}

				edges.push_back(edge);
			}
		}
	}
}

void drawEdges(cv::Mat& img_edge, std::vector<Edge>& edges)
{
	size_t length = edges.size();
	for (int i = 0; i < length; i++) {
		for (cv::Point pos : edges[i].points) {
			cv::Point3_<uchar>* p = img_edge.ptr<cv::Point3_<uchar>>(pos.y, pos.x);
			p->x = (i + 1) * 25 % 255;
			p->y = (i + 1) * 100 % 255;
			p->z = (i + 1) * 180 % 255;
		}
	}
}

void drawCurves(cv::Mat& img_curve, std::vector<Edge>& edges)
{
	for (Edge edge : edges) {
		if (edge.isCurve && edge.points.size() > 2) {
			edge.bezierFit();

			cv::Point points[1][5];
			points[0][0] = edge.points.front();
			points[0][1] = edge.control1;
			points[0][2] = edge.control2;
			points[0][3] = edge.points.back();

			const cv::Point* ppt[1] = { points[0] };
			int npt[] = { 4 }; 
			std::cout << points[0][0] << " " << points[0][1] << " " << points[0][2] << " " << points[0][3] << "\n";

			cv::polylines(img_curve, ppt, npt, 1, false, 255, 1, 8, 0);
		}
		else {
			cv::line(img_curve, edge.points.front(), edge.points.back(), 255, 1, 8, 0);
		}
	}
}