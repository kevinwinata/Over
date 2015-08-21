#include "tracing.h"
#include "utils.h"
#include <array>
#include <stack>
#include <iostream>
#include <fstream>
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
	std::array<int, 8> dir_x = { 1, 1, 0, -1, -1, -1, 0, 1 };
	std::array<int, 8> dir_y = { 0, -1, -1, -1, 0, 1, 1, 1 };

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
						int ypos = p->y + dir_y[n], xpos = p->x + dir_x[n];
						if (legalPoint(ypos, xpos, rows, cols) && contour[ypos][xpos]) {
							p = &(cv::Point(xpos, ypos));
							found = true;
							break;
						}
					}

					if (!found || (std::abs(dir_x[n] - dir_x[prevdir]) + std::abs(dir_y[n] - dir_y[prevdir]) > 2) ) {
						stop = true;
					}
					prevdir = n;
				}
				//edge.bezierFit();
				edges.push_back(edge);
			}
		}
	}
}

void contourChainCode(std::vector<std::vector<char>>& contour, int rows, int cols)
{
	std::array<char, 8> dir_x = { 1, 1, 0, -1, -1, -1, 0, 1 };
	std::array<char, 8> dir_y = { 0, -1, -1, -1, 0, 1, 1, 1 };

	cv::Mat img_edge = cv::Mat::zeros(rows, cols, CV_8UC3);
	std::vector<std::vector<std::pair<cv::Point, int>>> chains;

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			if (contour[i][j] != 0) {
				//if (contour[i][j] == 2) std::cout << i << " " << j << "\n";
				std::vector<std::pair<cv::Point, int>> chain;
				cv::Point* p = &(cv::Point(j, i));

				bool stop = false;
				int prev_ai = -1;

				while (!stop) {
					chain.push_back(std::make_pair(*p, prev_ai));

					contour[p->y][p->x] = 0;

					int n = 0;
					bool found = false;
					int x, y;

					for (int ai = 0; ai < 8; ai++) {
						int ypos = p->y + dir_y[ai], xpos = p->x + dir_x[ai];
						if (legalPoint(ypos, xpos, rows, cols) && contour[ypos][xpos] == 1) {
							x = xpos, y = ypos;
							prev_ai = ai;
							n++;
							found = true;
						}
					}
					if (n > 1) contour[p->y][p->x] = 2;
					if (found) p = &(cv::Point(x, y));

					stop = !found;
				}
				chains.push_back(chain);
			}
		}
	}

	size_t length = chains.size();
	for (int i = 0; i < length; i++) {
		for (auto pos : chains[i]) {
			cv::Point3_<uchar>* p = img_edge.ptr<cv::Point3_<uchar>>(pos.first.y, pos.first.x);
			p->x = (i + 1) * 25 % 255;
			p->y = (i + 1) * 100 % 255;
			p->z = (i + 1) * 180 % 255;
			//std::cout << pos.second;
		}
		//std::cout << "\n\n";
	}
	cv::namedWindow("Edges", CV_WINDOW_AUTOSIZE);
	cv::imshow("Edges", img_edge);
	findCorner(chains, 0.8, 4);
}

void findCorner(std::vector<std::vector<std::pair<cv::Point, int>>> chains, double threshold, int n)
{
	cv::Mat img_edge = cv::Mat::zeros(512, 512, CV_8UC1);
	for (auto chain : chains) {
		int length = (int)chain.size();
		for (int i = 1; i < length; i++) {
			int d1 = std::abs(chain[std::min(i+1, length-1)].second - chain[i].second);
			if (d1 > 4) d1 = 8 - d1;
			int k = std::abs(chain[std::min(i+2, length-1)].second - chain[std::max(i-1, 0)].second);
			if (k > 4) k = 8 - k;
			int d2 = d1 + k;
			
			if (d1 > 2) {
				std::cout << chain[i].first << "\n"; 
				*(img_edge.ptr<uchar>(chain[i].first.y, chain[i].first.x)) = 255;
			}
			else if (d1 == 1 || d1 == 2) {
				if (d2 > 3) {
					std::cout << chain[i].first << "\n";
					*(img_edge.ptr<uchar>(chain[i].first.y, chain[i].first.x)) = 255;
				}
				else if (d2 == 3) {
					int dify1 = chain[std::min(i+n, length-1)].first.y - chain[i].first.y;
					int difx1 = chain[std::min(i+n, length-1)].first.x - chain[i].first.x;
					double alpha1 = std::atan(dify1 / difx1);

					int dify2 = chain[i].first.y - chain[std::max(i-n, 0)].first.y;
					int difx2 = chain[i].first.x - chain[std::max(i-n, 0)].first.x;
					double alpha2 = std::atan(dify2 / difx2);

					if (std::abs(alpha1 - alpha2) > threshold) {
						std::cout << chain[i].first << "\n";
						*(img_edge.ptr<uchar>(chain[i].first.y, chain[i].first.x)) = 255;
					}
				}
			}
		}
	}
	cv::namedWindow("Corner", CV_WINDOW_AUTOSIZE);
	cv::imshow("Corner", img_edge);
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

void writeVector(std::string filename, std::vector<Edge>& edges, int width, int height)
{
	std::ofstream file;
	file.open(filename);

	file << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
	file << "<!DOCTYPE svg PUBLIC \" -//W3C//DTD SVG 1.1//EN\" ";
	file << "\"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n";

	file << "<svg version=\"1.1\"  xmlns=\"http://www.w3.org/2000/svg\" ";
	file << "width=\"" << width << "px\" ";
	file << "height = \"" << height << "\" ";
	file << "viewBox=\"0 0 " << width << " " << height << "\" xml:space=\"preserve\">\n";

	for (Edge edge : edges) {
		file << "<path d=\"";
		file << "M " << edge.points.front().x << " " << edge.points.front().y << " ";

		if (edge.isCurve && edge.points.size() > 2) {
			file << "C " <<
				edge.control1.x << " " << edge.control1.y << " " <<
				edge.control2.x << " " << edge.control2.y << " " <<
				edge.points.back().x << " " << edge.points.back().y << " ";
		}
		else {
			file << "L " << edge.points.back().x << " " << edge.points.back().y << " ";
		}
		file << "\" fill=\"none\" stroke=\"#000000\"/>\n";
	}
	file << "</svg>";
	file.close();
}