#include "vectortree.h"
#include <queue>
#include <iostream>

VectorTree::VectorTree(int n)
{
	this->n = n;
	adj.resize(n);
}

bool VectorTree::addEdge(int from, int to)
{
	std::queue<int> queue;
	queue.push(to);
	std::vector<bool> visited;
	visited.resize(n, false);
	bool found = false;

	while (!queue.empty() && !found) {
		int curnode = queue.front();
		queue.pop();
		visited[curnode] = true;

		found = (curnode == from);
		for (int idx : adj[curnode]) {
			if (!visited[idx]) queue.push(idx);
		}
	}

	if (!found) {
		adj[from].push_back(to);
		std::cout << "parent : " << from << ", child : " << to << "\n";
	}
	return !found;
}

void VectorTree::buildTree(std::vector<Region>& regions)
{
	std::vector<int> backgrounds;
	backgrounds.push_back(0);

	int size = (int)regions.size();
	for (int i = 0; i < size; i++) {
		bool found = false;

		for (int j = 0; j < size; j++) {
			bool match = false;
			for (int edge : regions[j].edges) {
				if (i != j && std::find(regions[i].edges.begin(), regions[i].edges.end(), edge) != regions[i].edges.end()) {
					match = true;
					break;
				}
			}
			if (match) {
				bool x = addEdge(i, j);
				//if (!x) std::cout << "x " << "\n";
				found = true;
			}
		}


		if (!found) {
			backgrounds.push_back(i);
			std::cout << "background : " << i << "\n";
		}
	}
}