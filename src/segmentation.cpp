#include "segmentation.h"
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <stack>
#include <array>

void colorMapSegmentation(cv::Mat& img, std::vector<std::vector<int>>& labels, std::vector<Region>& regions, int maxDistance)
{
	std::stack<std::pair<int, int>> stack;
	long curlab = 0;
	regions.clear();

	labels.resize(img.rows, std::vector<int>(img.cols, 0));

	std::array<std::pair<int, int>, 4> dir;
	dir[0] = std::make_pair(-1, 0);
	dir[1] = std::make_pair(1, 0);
	dir[2] = std::make_pair(0, -1);
	dir[3] = std::make_pair(0, 1);

	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			if (labels[i][j] == 0) {

				stack.push(std::make_pair(i, j));

				Region curreg;
				regions.push_back(curreg);
				Region& reg = regions.back();

				curlab++;

				while (!stack.empty()) {
					std::pair<int, int> pos = stack.top(); stack.pop();
					cv::Point3_<uchar>* pixel = img.ptr<cv::Point3_<uchar>>(pos.first, pos.second);
					labels[pos.first][pos.second] = curlab;

					reg.addPixel(*pixel);

					cv::Point3_<uchar>* temp;

					for (std::pair<int, int> p : dir) {
						int ypos = pos.first + p.first;
						int xpos = pos.second + p.second;
						if (ypos >= 0 && ypos < img.rows &&
							xpos >= 0 && xpos < img.cols &&
							labels[ypos][xpos] == 0)
						{
							temp = img.ptr<cv::Point3_<uchar>>(ypos, xpos);
							int dist = reg.countDiffToAvg(*temp);
							if (dist <= maxDistance) {
								stack.push(std::make_pair(ypos, xpos));
							}
						}
					}
				}
			}
		}
	}

	std::cout << curlab << std::endl;
}

void drawSegments(cv::Mat& img_seg, std::vector<std::vector<int>>& labels)
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