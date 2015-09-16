#include "output.h"
#include "utils.h"
#include <opencv2/imgproc.hpp>
#include <queue>
#include <iostream>
#include <fstream>

void drawSegments(cv::Mat& img_seg, std::vector<std::vector<long>>& labels)
{
	for (int i = 0; i < img_seg.rows; i++) {
		for (int j = 0; j < img_seg.cols; j++) {
			cv::Point3_<uchar>* p = img_seg.ptr<cv::Point3_<uchar>>(i, j);
			p->x = labels[i][j] * 25 % 255;
			p->y = labels[i][j] * 100 % 255;
			p->z = labels[i][j] * 180 % 255;
		}
	}
}

void drawContour(cv::Mat& img_contour, std::vector<std::vector<char>>& contour)
{
	for (int i = 0; i < img_contour.rows; i++) {
		for (int j = 0; j < img_contour.cols; j++) {
			if (contour[i][j] == 1) {
				*(img_contour.ptr<uchar>(i, j)) = 255;
			}
		}
	}
}

void drawChains(cv::Mat& img_chain, std::vector<std::vector<std::pair<cv::Point, int>>>& chains)
{
	size_t length = chains.size();
	for (int i = 0; i < length; i++) {
		for (auto pos : chains[i]) {
			cv::Point3_<uchar>* p = img_chain.ptr<cv::Point3_<uchar>>(pos.first.y, pos.first.x);
			p->x = (i + 1) * 55 % 255;
			p->y = (i + 1) * 78 % 255;
			p->z = (i + 1) * 134 % 255;
			//std::cout << pos.second;
		}
		//std::cout << "\n\n";
	}
}

void drawEdges(cv::Mat& img_edge, std::vector<std::vector<std::pair<cv::Point, int>>>& chains, std::vector<Path>& paths)
{
	std::array<char, 9> dir_x = { 1, 1, 0, -1, -1, -1, 0, 1, 0 };
	std::array<char, 9> dir_y = { 0, -1, -1, -1, 0, 1, 1, 1, 0 };

	drawChains(img_edge, chains);
	size_t length = paths.size();
	for (int i = 0; i < length; i++) {
		for (cv::Point pos : paths[i].corners) {
			for (int j = 0; j < 9; j++) {
				int posy = pos.y + dir_y[j]; int posx = pos.x + dir_x[j];
				if (legalPoint(posy, posx, img_edge.rows, img_edge.cols)) {
					cv::Point3_<uchar>* p = img_edge.ptr<cv::Point3_<uchar>>(posy, posx);
					p->x = 255; p->y = 255; p->z = 255;
				}
			}
		}
	}
}

void drawCurves(cv::Mat& img_curve, std::vector<Path>& paths)
{
	for (Path path : paths) {
		if (path.isCurve && path.corners.size() > 2) {
			path.bezierFit();

			cv::Point points[1][5];
			points[0][0] = path.corners.front();
			points[0][1] = path.control1;
			points[0][2] = path.control2;
			points[0][3] = path.corners.back();

			const cv::Point* ppt[1] = { points[0] };
			int npt[] = { 4 };
			std::cout << points[0][0] << " " << points[0][1] << " " << points[0][2] << " " << points[0][3] << "\n";

			cv::polylines(img_curve, ppt, npt, 1, false, 255, 1, 8, 0);
		}
		else {
			cv::line(img_curve, path.corners.front(), path.corners.back(), 255, 1, 8, 0);
		}
	}
}

void writeEdgeVector(std::string filename, std::vector<Path>& paths, int width, int height)
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

	for (Path path : paths) {
		file << "<path d=\"";

		bool first = true;
		for (cv::Point corner : path.corners) {
			file << ((first) ? "M " : "L ") << corner.x << " " << corner.y << " ";
			first = false;
		}
		file << "\" fill=\"none\" stroke=\"#000000\"/>\n";
	}
	file << "</svg>";
	file.close();
}

void writeVector(std::string filename, std::vector<Region>& regions, std::vector<Path>& paths, int width, int height)
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

	int regionsize = (int)regions.size(); 
	int max_n = 0; int max_idx = 0;
	for (int m = 0; m < regionsize; m++) {
		if (regions[m].n > max_n) {
			max_n = regions[m].n;
			max_idx = m;
		}
	}
	file << "<rect x=\"0\" y=\"0\" width = \"" << width << "\" height = \"" << height << "\" fill=\"" << regions[max_idx].getAvgColor() << "\" stroke=\"none\" />\n";

	for (int m = 0; m < regionsize; m++) {
		Region& region = regions[m];
		int totalpoints = 0;
		for (int pt : region.edges) totalpoints += paths[pt].corners.size();

		if (totalpoints > 3) {
			file << "<path d=\"";
			bool first = true;
			//bool isHollow = false;
			cv::Point ref;
			if (!region.edges.empty()) {
				ref = paths[region.edges[0]].corners[0];
			}

			int edgesize = (int)region.edges.size();
			for (int n = 0; n < edgesize; n++) {
				int idx = region.edges[n];
				Path& path = paths[idx];
				int size = (int)path.corners.size();
				bool reverse = std::find(region.reversed.begin(), region.reversed.end(), idx) != region.reversed.end();
				bool disconnect = std::find(region.disconnected.begin(), region.disconnected.end(), n-1) != region.disconnected.end();

				if (first) {
					file << "M " << path.corners[0].x << " " << path.corners[0].y << " ";
					first = false;
				}
				if (disconnect) {
					file << "\" fill=\"" << region.getAvgColor() << "\" stroke=\"none\"";
					file << "/>\n";
					file << "<path d=\"";
					file << "M " << path.corners[0].x << " " << path.corners[0].y << " ";
				}

				for (int i = 0; i < size; i++) {
					cv::Point& corner = (reverse) ? path.corners[size - 1 - i] : path.corners[i];
					file << "L " << corner.x << " " << corner.y << " ";
					first = false;
				}
			}

			file << "\" fill=\"" << region.getAvgColor() << "\" stroke=\"none\"";
			//if (isHollow) file << " fill-rule=\"evenodd\"";
			file << "/>\n";
		}
	}
	file << "</svg>";
	file.close();
}

void writeOptimizedVector(std::string filename, std::list<int>& sortedregions, std::vector<Region>& regions, std::vector<Path>& paths, int width, int height)
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

	int regionsize = (int)regions.size();
	int max_n = 0; int max_idx = 0;
	for (int m = 0; m < regionsize; m++) {
		if (regions[m].n > max_n) {
			max_n = regions[m].n;
			max_idx = m;
		}
	}
	file << "<rect x=\"0\" y=\"0\" width = \"" << width << "\" height = \"" << height << "\" fill=\"" << regions[max_idx].getAvgColor() << "\" stroke=\"none\" />\n";

	for (int reg : sortedregions) {

		Region& region = regions[reg];

		int totalpoints = 0;
		for (int pt : region.edges) totalpoints += paths[pt].corners.size();

		if (totalpoints > 3) {
			file << "<path d=\"";
			bool first = true;
			//bool isHollow = false;
			cv::Point ref;
			if (!region.edges.empty()) {
				ref = paths[region.edges[0]].corners[0];
			}

			int edgesize = (int)region.edges.size();
			for (int n = 0; n < edgesize; n++) {
				int idx = region.edges[n];
				Path& path = paths[idx];
				int size = (int)path.corners.size();
				bool reverse = std::find(region.reversed.begin(), region.reversed.end(), idx) != region.reversed.end();
				bool disconnect = std::find(region.disconnected.begin(), region.disconnected.end(), n - 1) != region.disconnected.end();

				if (first) {
					file << "M " << path.corners[0].x << " " << path.corners[0].y << " ";
					first = false;
				}
				if (disconnect) {
					file << "\" fill=\"" << region.getAvgColor() << "\" stroke=\"none\"";
					file << "/>\n";
					file << "<path d=\"";
					file << "M " << path.corners[0].x << " " << path.corners[0].y << " ";
				}

				for (int i = 0; i < size; i++) {
					cv::Point& corner = (reverse) ? path.corners[size - 1 - i] : path.corners[i];
					if (std::find(region.deletelist.begin(), region.deletelist.end(), corner) == region.deletelist.end()) {
						file << "L " << corner.x << " " << corner.y << " ";
					}
					first = false;
				}
			}

			file << "\" fill=\"" << region.getAvgColor() << "\" stroke=\"none\"";
			//if (isHollow) file << " fill-rule=\"evenodd\"";
			file << "/>\n";
		}
	}

	file << "</svg>";
	file.close();
}