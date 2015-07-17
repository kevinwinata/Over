#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include "region.h"
#include "segmentation.h"

cv::Mat img;
int maxDistance = 40;
std::vector<std::vector<int>> labels;
std::vector<Region> regions;

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		return -1;
	}

	img = cv::imread(argv[1], cv::IMREAD_COLOR); // Read the file

	if (!img.data) // Check for invalid input
	{
		std::cout << "Could not open or find the image\n";
		return -1;
	}

	cv::namedWindow("Source", cv::WINDOW_AUTOSIZE);
	cv::imshow("Source", img);	

	colorMapSegmentation(img, labels, regions, maxDistance);

	cv::Mat img_seg(img.rows, img.cols, img.type());
	drawSegments(img_seg, labels);

	cv::namedWindow("Segmentated", CV_WINDOW_AUTOSIZE);
	cv::imshow("Segmentated", img_seg);
	
	cv::waitKey(0); // Wait for a keystroke in the window
	return 0;
}