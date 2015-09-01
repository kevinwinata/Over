#ifndef VECTOR_TREE_H
#define VECTOR_TREE_H

#include "region.h"
#include <vector>

class VectorTree
{
public:
	int n;
	std::vector<std::vector<int>> adj;

	VectorTree(int n);
	bool addEdge(int from, int to);
	void buildTree(std::vector<Region>& regions);
	void optimize(std::vector<Region>& regions, std::vector<int>& backgrounds, std::vector<Path>& paths);
};

#endif