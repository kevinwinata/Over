#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include "region.h"
#include "segmentation.h"
#include "tracing.h"

cv::Mat img;
int maxDistance = 15;
std::vector<std::vector<long>> labels;
std::vector<std::vector<bool>> contour;
std::vector<Region> regions;
std::vector<std::vector<cv::Point>> edges;

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

	cv::Mat img_contour = cv::Mat::zeros(img.rows, img.cols, CV_8UC1);
	findContour(labels, contour, img.rows, img.cols);
	drawContour(img_contour, contour);
	cv::namedWindow("Contour", CV_WINDOW_AUTOSIZE);
	cv::imshow("Contour", img_contour);

	cv::Mat img_edge = cv::Mat::zeros(img.rows, img.cols, CV_8UC3);
	separateEdges(contour, edges, img.rows, img.cols);
	drawEdges(img_edge, edges);
	cv::namedWindow("Edges", CV_WINDOW_AUTOSIZE);
	cv::imshow("Edges", img_edge);
	
	cv::waitKey(0); // Wait for a keystroke in the window
	return 0;
}