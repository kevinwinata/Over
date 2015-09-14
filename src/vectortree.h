#ifndef VECTOR_TREE_H
#define VECTOR_TREE_H

#include "region.h"
#include <vector>
#include <list>

class VectorTree
{
public:
	int n;
	std::vector<std::vector<int>> adj;
	std::vector<bool> is_backgrounds;

	VectorTree(int n);
	bool addEdge(int from, int to);
	void buildTree(std::vector<Region>& regions, std::vector<int>& backgrounds);
	void optimize(std::vector<Region>& regions, std::vector<Path>& paths, std::vector<std::vector<long>>& labels);
	void VectorTree::topologicalSort(std::list<int>& sortedregions);
};

#endif