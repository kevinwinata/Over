#ifndef VECTOR_TREE_H
#define VECTOR_TREE_H

#include <vector>

class VectorTree
{
public:
	int n;
	std::vector<std::vector<int>> adj;

	VectorTree(int n);
	void addEdge(int from, int to);
};

#endif