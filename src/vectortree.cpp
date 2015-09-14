#include "vectortree.h"
#include "utils.h"
#include <queue>
#include <stack>
#include <iostream>

VectorTree::VectorTree(int n)
{
	this->n = n;
	adj.resize(n);
	is_backgrounds.resize(n, true);
}

bool VectorTree::addEdge(int from, int to)
{
	/*std::queue<int> queue;
	queue.push(to);
	std::vector<bool> visited;
	visited.resize(n, false);
	bool found = false;

	while (!queue.empty() && !found) {
		int curnode = queue.front();
		//std::cout << curnode << "\n";
		queue.pop();
		visited[curnode] = true;

		found = (curnode == from);
		for (int idx : adj[curnode]) {
			if (!visited[idx]) queue.push(idx);
		}
	}*/
	bool legal = from != to && 
		std::find(adj[to].begin(), adj[to].end(), from) == adj[to].end() && 
		std::find(adj[from].begin(), adj[from].end(), to) == adj[from].end();

	if (legal) {
		adj[from].push_back(to);
		is_backgrounds[to] = false;
		//std::cout << "parent : " << from << ", child : " << to << "\n";
	}
	else {
		//std::cout << "cant add edge " << from << ", child : " << to << "\n";
	}
	return legal;
}

void VectorTree::buildTree(std::vector<Region>& regions, std::vector<int>& backgrounds)
{
	backgrounds.push_back(0);

	int size = (int)regions.size();
	for (int i = 0; i < size; i++) {
		bool found = false;

		for (int j = 0; j < size; j++) {
			std::cout << "current : " << i << " " << j << "\n";
			bool match = false;
			for (int edge : regions[j].edges) {
				if (i != j && std::find(regions[i].edges.begin(), regions[i].edges.end(), edge) != regions[i].edges.end()) {
					match = true;
					break;
				}
			}
			if (match) {
				std::cout << "match\n";
				bool x = addEdge(i, j);
				//if (!x) std::cout << "x " << "\n";
				found = true;
			}
		}

		if (!found) {
			backgrounds.push_back(i);
			//std::cout << "background : " << i << "\n";
		}
	}
}

void VectorTree::topologicalSort(std::list<int>& sortedregions)
{
	std::vector<bool> visited;
	visited.resize(n, false);
	std::vector<bool> inserted;
	inserted.resize(n, false);

	std::stack<std::pair<bool, int>> node_stack;
	std::vector<int>::iterator it;
	std::vector<std::vector<int> > graph;

	for (int i = 0; i < n; i++){
		if (visited[i] == false){
			node_stack.push(std::make_pair(false, i));
		}
		while (!node_stack.empty()){
			std::pair<bool, int> node = node_stack.top();
			node_stack.pop();
			if (node.first) {
				if (!inserted[node.second]) {
					sortedregions.push_front(node.second);
					inserted[node.second] = true;
				}
				continue;
			}
			visited[node.second] = true;
			node_stack.push(std::make_pair(true, node.second));
			for (int child : adj[node.second]){
				if (visited[child] == false){
					node_stack.push(std::make_pair(false, child));
				}
			}
		}
	}
}

void VectorTree::optimize(std::vector<Region>& regions, std::vector<Path>& paths, std::vector<std::vector<long>>& labels)
{
	int size = (int)regions.size();
	for (int i = 0; i < size; i++) {

		for (int child : adj[i]) {
			//std::cout << "simplifying region " << i << " from " << child << "\n";
			simplify(regions, i, child, paths, labels);
		}
	}
}