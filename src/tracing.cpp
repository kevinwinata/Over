#include "tracing.h"
#include "utils.h"
#include <array>
#include <stack>
#include <iostream>
#include <opencv2/highgui/highgui.hpp>

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

void findCorner(std::vector<std::vector<std::pair<cv::Point, int>>>& chains, std::vector<Path>& paths, double threshold, int n)
{
	//cv::Mat img_edge = cv::Mat::zeros(512, 512, CV_8UC1);

	for (auto chain : chains) {
		Path path;
		path.addCorner(cv::Point(chain[0].first.x, chain[0].first.y));

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
					path.addCorner(cv::Point(chain[i].first.x, chain[i].first.y));
					//*(img_edge.ptr<uchar>(chain[i].first.y, chain[i].first.x)) = 255;
				}
				else if (d1 == 1 || d1 == 2) {
					if (d2 > 3) {
						//std::cout << chain[i].first << "\n";
						path.addCorner(cv::Point(chain[i].first.x, chain[i].first.y));
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
							path.addCorner(cv::Point(chain[i].first.x, chain[i].first.y));
							//*(img_edge.ptr<uchar>(chain[i].first.y, chain[i].first.x)) = 255;
						}
					}
				}
				/*if (std::abs(chain[std::min(i + 1, length - 1)].second != chain[i].second)) {
					path.addCorner(cv::Point(chain[i].first.x, chain[i].first.y));
				}*/
			}
			path.addCorner(cv::Point(chain[length - 1].first.x, chain[length - 1].first.y));
		}
		paths.push_back(path);
	}
	//cv::namedWindow("Corner", CV_WINDOW_AUTOSIZE);
	//cv::imshow("Corner", img_edge);
}

void edgeSort(std::vector<Region>& regions, std::vector<Path>& paths, int t)
{
	for (Region& region : regions) {
		int size = (int)region.edges.size();

		for (int i = 0; i < size - 1; i++) {
			Path& path = paths[region.edges[i]];
			bool reverse = std::find(region.reversed.begin(), region.reversed.end(), region.edges[i]) != region.reversed.end();
			cv::Point p = (reverse) ? path.corners.front() : path.corners.back();

			int minidx = i;
			int mindist = 65536;
			bool front = true;

			for (int idx = i + 1; idx < size; idx++) {
				cv::Point front = paths[region.edges[idx]].corners.front();
				int dist = std::abs(p.x - front.x) + std::abs(p.y - front.y);

				if (mindist > dist) {
					mindist = dist;
					minidx = idx;
				}
			}

			for (int idx = i + 1; idx < size; idx++) {
				cv::Point back = paths[region.edges[idx]].corners.back();
				int dist = std::abs(p.x - back.x) + std::abs(p.y - back.y);

				if (mindist > dist) {
					front = false;
					mindist = dist;
					minidx = idx;
				}
			}

			if (mindist < t) {
				if (!front) region.reversed.push_back(region.edges[minidx]);
				int temp = region.edges[minidx];
				region.edges[minidx] = region.edges[i+1];
				region.edges[i+1] = temp;

				//int c = (!matchfront && matchback) ? paths[temp].corners.size() - 1 : 0;
				//paths[temp].corners[c] = p;
			}
			else {
				region.disconnected.push_back(region.edges[i]);
			}
		}
	}
}