#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <array>
#include <list>
#include <cstdio>
#include <ctime>
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
std::vector<int> backgrounds;
std::list<int> sortedregions;
std::clock_t prev;
double duration;

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

	std::string filename = std::string(argv[1]) + ".png";
	img = cv::imread(filename, cv::IMREAD_COLOR); // Read the file

	if (!img.data) // Check for invalid input
	{
		std::cout << "Could not open or find the image\n";
		return -1;
	}

	/* Your algorithm here */


	/*cv::namedWindow("Source", cv::WINDOW_AUTOSIZE);
	cv::imshow("Source", img);*/

	std::cout << "Segmentating ... \n";
	prev = std::clock();

	colorMapSegmentation(img, labels, regions, maxDistance);
	cleanNoise(labels, regions, img.rows, img.cols, 20);
	VectorTree tree(regions.size());

	duration = (std::clock() - prev) / (double)CLOCKS_PER_SEC;
	std::cout << "time elapsed : " << duration << '\n';

	/*cv::Mat img_seg(img.rows, img.cols, img.type());
	drawSegments(img_seg, labels);
	cv::namedWindow("Segmentated", CV_WINDOW_AUTOSIZE);
	cv::imshow("Segmentated", img_seg);*/


	std::cout << "\nDetecting contours ... \n";
	prev = std::clock();
	cv::Mat img_contour = cv::Mat::zeros(img.rows, img.cols, CV_8UC1);
	findContour(labels, contour, img.rows, img.cols);

	duration = (std::clock() - prev) / (double)CLOCKS_PER_SEC;
	std::cout << "time elapsed : " << duration << '\n';

	/*drawContour(img_contour, contour);
	cv::namedWindow("Contour", CV_WINDOW_AUTOSIZE);
	cv::imshow("Contour", img_contour);*/


	std::cout << "\nSeparating edges ... \n";
	prev = std::clock();
	contourChainCode(contour, chains, labels, regions, tree, img.rows, img.cols);

	duration = (std::clock() - prev) / (double)CLOCKS_PER_SEC;
	std::cout << "time elapsed : " << duration << '\n';

	/*cv::Mat img_chain = cv::Mat::zeros(img.rows, img.cols, CV_8UC3);
	drawChains(img_chain, chains);
	cv::namedWindow("Chain Code", CV_WINDOW_AUTOSIZE);
	cv::imshow("Chain Code", img_chain);*/


	std::cout << "\nDetecting corners ... \n";
	prev = std::clock();
	findCorner(chains, paths, 0.2, 2);
	std::cout << paths.size() << "\n";

	duration = (std::clock() - prev) / (double)CLOCKS_PER_SEC;
	std::cout << "time elapsed : " << duration << '\n';

	/*cv::Mat img_edge = cv::Mat::zeros(img.rows, img.cols, CV_8UC3);
	drawEdges(img_edge, chains, paths);
	cv::namedWindow("Corners", CV_WINDOW_AUTOSIZE);
	cv::imshow("Corners", img_edge);*/


	std::cout << "\nSorting edges ... \n";
	prev = std::clock();
	edgeSort(regions, paths, 10);

	duration = (std::clock() - prev) / (double)CLOCKS_PER_SEC;
	std::cout << "time elapsed : " << duration << '\n';


	std::cout << "\nOptimizations ... \n";
	prev = std::clock();

	tree.topologicalSort(sortedregions);

	tree.optimize(regions, paths, labels);

	duration = (std::clock() - prev) / (double)CLOCKS_PER_SEC;
	std::cout << "time elapsed : " << duration << '\n';

	/*cv::setMouseCallback("Edges", mouseCallback, NULL);
	cv::setMouseCallback("Corners", posCallback, NULL);*/

	std::cout << "\nOutputting SVG ... \n";

	prev = std::clock();
	std::string vectorfile = std::string(argv[1]) + ".svg";
	writeVector(vectorfile, regions, paths, img.cols, img.rows);
	duration = (std::clock() - prev) / (double)CLOCKS_PER_SEC;
	std::cout << "time elapsed : " << duration << '\n';

	prev = std::clock();
	std::string optimizedfile = std::string(argv[1]) + "-opt.svg";
	writeOptimizedVector(optimizedfile, sortedregions, regions, paths, img.cols, img.rows);
	duration = (std::clock() - prev) / (double)CLOCKS_PER_SEC;
	std::cout << "time elapsed : " << duration << '\n';

	std::cout << "\nDone.";
	return 0;
}