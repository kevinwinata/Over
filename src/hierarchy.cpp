#include "hierarchy.h"
#include <queue>
#include <iostream>

void buildTree(VectorTree& tree, std::vector<Region>& regions)
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
				bool x = tree.addEdge(j, i);
				if (!x) std::cout << "x "<< "\n";
				found = true;
			}
		}
		

		if (!found) {
			backgrounds.push_back(i);
			std::cout << "background : " << i << "\n";
		}
	}
}