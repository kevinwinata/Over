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
	findCorner(chains, paths, 0, 6);

	cv::Mat img_chain = cv::Mat::zeros(img.rows, img.cols, CV_8UC3);
	drawChains(img_chain, chains);
	cv::namedWindow("Chain Code", CV_WINDOW_AUTOSIZE);
	cv::imshow("Chain Code", img_chain);

	cv::Mat img_edge = cv::Mat::zeros(img.rows, img.cols, CV_8UC3);
	drawEdges(img_edge, chains, paths);
	cv::namedWindow("Corners", CV_WINDOW_AUTOSIZE);
	cv::imshow("Corners", img_edge);

	/*for (int i = 0; i < regions.size(); i++) {
		std::cout << "region " << i << "\n";
		for (auto e : regions[i].edges) {
			std::cout << e << " ";
		}
		std::cout << "\n";
	}*/

	std::cout << "\nSorting edges ... \n";
	edgeSort(regions, paths, 5);

	/*for (int i = 0; i < regions.size(); i++) {
		std::cout << "region " << i << "\nedges : ";
		for (auto e : regions[i].edges) {
			std::cout << e << " ";
		}
		std::cout << "\nreversed : ";
		for (auto e : regions[i].reversed) {
			std::cout << e << " ";
		}
		std::cout << "\ndisconnected : ";
		for (auto e : regions[i].disconnected) {
			std::cout << e << " ";
		}
		std::cout << "\n";
	}*/
	VectorTree tree(regions.size());
	tree.buildTree(regions);
	std::vector<int> xx; xx.push_back(0);
	tree.optimize(regions, xx, paths);

	cv::setMouseCallback("Edges", mouseCallback, NULL);
	cv::setMouseCallback("Corners", posCallback, NULL);

	std::cout << "\nOutputting SVG ... \n";
	writeVector("example.svg", regions, paths, img.cols, img.rows);
	writeOptimizedVector("example-optimized.svg", tree, regions, paths, img.cols, img.rows);

	std::cout << "\nDone.";
	cv::waitKey(0); // Wait for a keystroke in the window
	return 0;
}