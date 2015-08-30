#include "vectortree.h"
#include <queue>
#include <iostream>

VectorTree::VectorTree(int n)
{
	this->n = n;
	adj.resize(n);
}

void VectorTree::addEdge(int from, int to)
{
	if (std::find(adj[from].begin(), adj[from].end(), to) == adj[from].end()) {
		adj[from].push_back(to);
		std::cout << "parent : " << from << ", child : " << to << "\n";
	}
}
