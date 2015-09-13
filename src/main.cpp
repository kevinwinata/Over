#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <array>
#include "region.h"
#include "path.h"
#include "vectortree.h"
#include "segmentation.h"
#include "tracing.h"
#include "output.h"

cv::Mat img;
int maxDistance = 15;
std::vector<std::vector<long>> labels;
std::vector<std::vector<char>> contour; 
std::vector<std::vector<std::pair<cv::Point, int>>> chains;
std::vector<Region> regions;
std::vector<Path> paths;

void mouseCallback(int event, int x, int y, int flags, void* userdata)
{
	if (event == cv::EVENT_LBUTTONDOWN)
	{
		Region& region = regions[labels[y][x] - 1];
		std::cout << "region : " << labels[y][x] << std::endl;
		region.printProps();
	}
}
void posCallback(int event, int x, int y, int flags, void* userdata)
{
	if (event == cv::EVENT_LBUTTONDOWN)
	{
		std::cout << x << " " << y << std::endl;
	}
}

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

	std::cout << "Segmentating ... \n";
	colorMapSegmentation(img, labels, regions, maxDistance);
	cv::Mat img_seg(img.rows, img.cols, img.type());
	drawSegments(img_seg, labels);
	cv::namedWindow("Segmentated", CV_WINDOW_AUTOSIZE);
	cv::imshow("Segmentated", img_seg);

	std::cout << "\nDetecting contours ... \n";
	cv::Mat img_contour = cv::Mat::zeros(img.rows, img.cols, CV_8UC1);
	findContour(labels, contour, img.rows, img.cols);
	drawContour(img_contour, contour);
	cv::namedWindow("Contour", CV_WINDOW_AUTOSIZE);
	cv::imshow("Contour", img_contour);

	std::cout << "\nSeparating edges ... \n";
	contourChainCode(contour, chains, labels, regions, img.rows, img.cols);

	std::cout << "\nDetecting corners ... \n";
	findCorner(chains, paths, 0.2, 2);

	cv::Mat img_chain = cv::Mat::zeros(img.rows, img.cols, CV_8UC3);
	drawChains(img_chain, chains);
	cv::namedWindow("Chain Code", CV_WINDOW_AUTOSIZE);
	cv::imshow("Chain Code", img_chain);

	cv::Mat img_edge = cv::Mat::zeros(img.rows, img.cols, CV_8UC3);
	drawEdges(img_edge, chains, paths);
	cv::namedWindow("Corners", CV_WINDOW_AUTOSIZE);
	cv::imshow("Corners", img_edge);

	std::cout << "\nSorting edges ... \n";
	edgeSort(regions, paths, 10);

	std::cout << "\nOptimizations ... \n";
	VectorTree tree(regions.size());
	std::vector<int> backgrounds;
	tree.buildTree(regions, backgrounds);
	tree.optimize(regions, backgrounds, paths, labels);

	cv::setMouseCallback("Edges", mouseCallback, NULL);
	cv::setMouseCallback("Corners", posCallback, NULL);

	std::cout << "\nOutputting SVG ... \n";
	writeVector("example.svg", regions, paths, img.cols, img.rows);
	writeOptimizedVector("example-optimized.svg", tree, backgrounds, regions, paths, img.cols, img.rows);

	std::cout << "\nDone.";
	cv::waitKey(0); // Wait for a keystroke in the window
	return 0;
}