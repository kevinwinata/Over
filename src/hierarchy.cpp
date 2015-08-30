#include "hierarchy.h"
#include <queue>
#include <iostream>

void buildTree(VectorTree& tree, std::vector<Region>& regions)
{
	std::vector<int> backgrounds;
	backgrounds.push_back(0);

	int size = (int)regions.size();
	for (int i = 1; i < size; i++) {
		bool found = false;

		// BFS
		// search for any node where regions[val] have same edges with regions[i]
		// if found background/node .addChild(i)
		std::vector<bool> visited;
		visited.resize(tree.n, false);

		for (int j : backgrounds) {
			std::queue<int> queue;
			queue.push(j);

			while (!queue.empty()) {
				int curnode = queue.front();
				queue.pop();
				visited[curnode] = true;

				//std::cout << curnode << " ";
				bool match = false;
				for (int edge : regions[curnode].edges) {
					if (i != curnode && std::find(regions[i].edges.begin(), regions[i].edges.end(), edge) != regions[i].edges.end()) {
						match = true;
						break;
					}
				}
				if (match) {
					tree.addEdge(curnode, i);
					found = true;
				}

				for (int idx : tree.adj[curnode]) {
					if (!visited[idx]) queue.push(idx);
				}
			}
		}
		

		if (!found) {
			backgrounds.push_back(i);
			std::cout << "background : " << i << "\n";
		}
	}
}