#pragma once
#include"Graph.h"

struct Graph_Edge {
	int start_node;
	int second_node;
	int length;
	int last_second_node;
	int end_node;
};

class Graph_Structure {
private:
	Graph_Edge (*edges)[SKELETONIZE::DEGREE_MAX];
	int *key_node_degree;
	int *hash_index;
	int node_num;
	int key_node_num;
public:
	Graph_Structure(int node_num, int key_node_num);
	~Graph_Structure();
	void Check_Structure_Node(int node_index);
};

class Graph_Prune {
private:
	const Graph * before_prune;
	Graph * after_prune;
	bool * node_available;
	int node_num;

	void Get_Largest_Subgraph();
	int Find_Leftmost_Point();
public:
	void Prune(const Graph * graph_before_prune, Graph * graph_after_prune);
};

