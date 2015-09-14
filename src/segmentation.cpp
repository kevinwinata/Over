#include "segmentation.h"
#include "utils.h"
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <stack>
#include <array>

void colorMapSegmentation(cv::Mat& img, std::vector<std::vector<long>>& labels, std::vector<Region>& regions, int maxDistance)
{
	std::stack<std::pair<int, int>> stack;
	int curlab = 0;
	regions.clear();

	labels.resize(img.rows, std::vector<long>(img.cols, 0));

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
						if (legalPoint(ypos, xpos, img.rows, img.cols) && labels[ypos][xpos] == 0)
						{
							temp = img.ptr<cv::Point3_<uchar>>(ypos, xpos);
							if (reg.countDiffToAvg(*temp) <= maxDistance) {
								stack.push(std::make_pair(ypos, xpos));
							}
						}
					}
				}
			}
		}
	}

	std::cout << "total regions : " << curlab << std::endl;
}

void cleanNoise(std::vector<std::vector<long>>& labels, std::vector<Region>& regions, int rows, int cols, int minSize)
{
	int rr = 0;
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			if (regions[labels[i][j] - 1].n < minSize && i > 0 && j > 0) {
				labels[i][j] = labels[i - 1][j - 1];
				regions[labels[i][j] - 1].relevant = false;
			}
		}
	}
	for (Region &r : regions) {
		if (r.relevant) rr++;
	}
	std::cout << "relevant regions : " << rr << "\n";
}
