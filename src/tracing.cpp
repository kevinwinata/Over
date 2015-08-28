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
					edge.addCorner(*p);
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

void contourChainCode(std::vector<std::vector<char>>& contour, std::vector<std::vector<std::pair<cv::Point, int>>>& chains, std::vector<std::vector<long>>& labels, std::vector<Region>& regions, int rows, int cols)
{
	std::array<char, 8> dir_x = { 1, 1, 0, -1, -1, -1, 0, 1 };
	std::array<char, 8> dir_y = { 0, -1, -1, -1, 0, 1, 1, 1 };

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			if (contour[i][j] != 0) {
				std::vector<std::pair<cv::Point, int>> chain;
				cv::Point* p = &(cv::Point(j, i));

				bool stop = false;
				int prev_ai = -1;
				long reg1 = labels[i][j];
				long reg2 = (i > 0 && j > 0) ? labels[i - 1][j - 1] : 0;

				while (!stop) {
					chain.push_back(std::make_pair(*p, prev_ai));

					contour[p->y][p->x]--;

					int n = 0;
					bool found = false;
					bool changereg = false;
					int x, y;

					for (int ai = 0; ai < 8; ai += 1) {
						int ypos = p->y + dir_y[ai], xpos = p->x + dir_x[ai];
						if (legalPoint(ypos, xpos, rows, cols)) {
							changereg = labels[ypos][xpos] != reg1;
							if (ypos != 0 && ypos != rows - 1 && xpos != 0 && xpos != cols - 1)
								changereg &= labels[ypos][xpos] != reg2;
							if (changereg) break;

							if (contour[ypos][xpos] > 0 && std::abs(prev_ai - ai) != 4) {
								x = xpos, y = ypos;
								prev_ai = ai;
								n++;
								found = true;
							}
						}
						//ai = (ai == 7) ? -2 : ai;
					}
					//if (n > 1) contour[p->y][p->x] ++;
					//if (changereg) contour[p->y][p->x]++;
					if (found) p = &(cv::Point(x, y));

					stop = !found || changereg;
				}

				regions[reg1 - 1].addEdge(chains.size());
				if (reg2 > 0) regions[reg2 - 1].addEdge(chains.size());
				chains.push_back(chain);
			}
		}
	}
}

void findCorner(std::vector<std::vector<std::pair<cv::Point, int>>>& chains, std::vector<Edge>& edges, double threshold, int n)
{
	//cv::Mat img_edge = cv::Mat::zeros(512, 512, CV_8UC1);

	for (auto chain : chains) {
		Edge edge;
		edge.addCorner(cv::Point(chain[0].first.x, chain[0].first.y));

		if (chain.size() > 1) {
			int length = (int)chain.size();
			for (int i = 1; i < length; i++) {
				int d1 = std::abs(chain[std::min(i + 1, length - 1)].second - chain[i].second);
				d1 = (d1 > 4) ? 8 - d1 : d1;
				int k = std::abs(chain[std::min(i + 2, length - 1)].second - chain[std::max(i - 1, 0)].second);
				k = (k > 4) ? 8 - k : k;
				int d2 = d1 + k;

				if (d1 > 2) {
					//std::cout << chain[i].first << "\n"; 
					edge.addCorner(cv::Point(chain[i].first.x, chain[i].first.y));
					//*(img_edge.ptr<uchar>(chain[i].first.y, chain[i].first.x)) = 255;
				}
				else if (d1 == 1 || d1 == 2) {
					if (d2 > 3) {
						//std::cout << chain[i].first << "\n";
						edge.addCorner(cv::Point(chain[i].first.x, chain[i].first.y));
						//*(img_edge.ptr<uchar>(chain[i].first.y, chain[i].first.x)) = 255;
					}
					else if (d2 == 3) {
						int dify1 = chain[std::min(i + n, length - 1)].first.y - chain[i].first.y;
						int difx1 = chain[std::min(i + n, length - 1)].first.x - chain[i].first.x;
						double dif1 = (difx1 == 0) ? INFINITY : (dify1 / difx1);
						double alpha1 = std::atan(dif1);

						int dify2 = chain[i].first.y - chain[std::max(i - n, 0)].first.y;
						int difx2 = chain[i].first.x - chain[std::max(i - n, 0)].first.x;
						double dif2 = (difx2 == 0) ? INFINITY : (dify2 / difx2);
						double alpha2 = std::atan(dif2);

						if (std::abs(alpha1 - alpha2) > threshold) {
							//std::cout << chain[i].first << "\n";
							edge.addCorner(cv::Point(chain[i].first.x, chain[i].first.y));
							//*(img_edge.ptr<uchar>(chain[i].first.y, chain[i].first.x)) = 255;
						}
					}
				}
			}
			edge.addCorner(cv::Point(chain[length - 1].first.x, chain[length - 1].first.y));
		}
		edges.push_back(edge);
	}
	//cv::namedWindow("Corner", CV_WINDOW_AUTOSIZE);
	//cv::imshow("Corner", img_edge);
}

void edgeSort(std::vector<Region>& regions, std::vector<Edge>& edges)
{
	//std::array<char, 8> dir_x = { 1, 1, 0, -1, -1, -1, 0, 1 };
	//std::array<char, 8> dir_y = { 0, -1, -1, -1, 0, 1, 1, 1 };

	for (Region& region : regions) {
		int size = (int)region.edges.size();
		for (int i = 0; i < size; i++) {
			Edge& edge = edges[region.edges[i]];
			bool reverse = std::find(region.reversed.begin(), region.reversed.end(), region.edges[i]) != region.reversed.end();
			cv::Point p = (reverse) ? edge.corners.front() : edge.corners.back();

			/*std::array<cv::Point, 9> sq;
			for (int j = 0; j < 8; j++) sq[j] = cv::Point(p.x + dir_x[j], p.y + dir_y[j]);
			sq[8] = p;*/

			int k = 1;
			bool found = false;
			bool matchfront, matchback;
			int idx;
			while (!found && k < size - 1) {
				idx = (i + k < size) ? i + k : i + k - size;
				cv::Point front = edges[region.edges[idx]].corners.front();
				//matchfront = std::find(sq.begin(), sq.end(), front) != sq.end();
				matchfront = (std::abs(p.x - front.x) < 3 && std::abs(p.y - front.y) < 3);

				cv::Point back = edges[region.edges[idx]].corners.back();
				//matchback = std::find(sq.begin(), sq.end(), back) != sq.end();
				matchback = (std::abs(p.x - back.x) < 3 && std::abs(p.y - back.y) < 3);

				found = matchfront || matchback;
				if (!matchfront && matchback) region.reversed.push_back(region.edges[idx]);
				/*if (!edges[region.edges[idx]].reverse) {
					edges[region.edges[idx]].reverse = !matchfront && matchback;
				}*/
				//else std::cout << region.edges[idx] << "\n";
				k++;
			}
			//std::cout << "edges " << i << " : ";
			if (found) {
				//int idx = (i + k - 1 < size) ? i + k - 1 : i + k - 1 - size;
				int next = (i + 1 < size) ? i + 1 : i + 1 - size;
				int temp = region.edges[idx];
				region.edges[idx] = region.edges[next];
				region.edges[next] = temp;
				//std::cout << "swapped " << region.edges[idx] << " with " << region.edges[next];

				int c = (!matchfront && matchback) ? edges[temp].corners.size() - 1 : 0;
				edges[temp].corners[c] = p;
			}
			//std::cout << "\n";
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

void drawEdges(cv::Mat& img_edge, std::vector<Edge>& edges)
{
	size_t length = edges.size();
	for (int i = 0; i < length; i++) {
		for (cv::Point pos : edges[i].corners) {
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
		if (edge.isCurve && edge.corners.size() > 2) {
			edge.bezierFit();

			cv::Point points[1][5];
			points[0][0] = edge.corners.front();
			points[0][1] = edge.control1;
			points[0][2] = edge.control2;
			points[0][3] = edge.corners.back();

			const cv::Point* ppt[1] = { points[0] };
			int npt[] = { 4 }; 
			std::cout << points[0][0] << " " << points[0][1] << " " << points[0][2] << " " << points[0][3] << "\n";

			cv::polylines(img_curve, ppt, npt, 1, false, 255, 1, 8, 0);
		}
		else {
			cv::line(img_curve, edge.corners.front(), edge.corners.back(), 255, 1, 8, 0);
		}
	}
}

void writeEdgeVector(std::string filename, std::vector<Edge>& edges, int width, int height)
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

		bool first = true;
		for (cv::Point corner : edge.corners) {
			file << ((first) ? "M " : "L ") << corner.x << " " << corner.y << " ";
			first = false;
		}
		file << "\" fill=\"none\" stroke=\"#000000\"/>\n";
	}
	file << "</svg>";
	file.close();
}

void writeVector(std::string filename, std::vector<Region>& regions, std::vector<Edge>& edges, int width, int height)
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

	for (Region region : regions) {
		file << "<path d=\"";
		bool first = true;

		for (int idx : region.edges) {
			Edge& edge = edges[idx];
			int size = (int)edge.corners.size();
			bool reverse = std::find(region.reversed.begin(), region.reversed.end(), idx) != region.reversed.end();
			for (int i = 0; i < size; i++) {
				cv::Point& corner = (reverse) ? edge.corners[size-1-i] : edge.corners[i];
				file << ((first) ? "M " : "L ") << corner.x << " " << corner.y << " ";
				first = false;
			}
			//if (reverse) file << " '" ;
			//file << "/" << idx << " ";
		}

		file << "Z\" fill=\"" << region.getAvgColor() << "\" stroke=\"none\"/>\n";
	}
	file << "</svg>";
	file.close();
}