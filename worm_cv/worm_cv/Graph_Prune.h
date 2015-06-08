#pragma once
#include"Graph.h"
#include"Graph_Structure.h"


class Graph_Prune {
private:
	const Graph * before_prune;
	Graph * after_prune;
	Graph_Structure * graph_structure;
	bool * node_available;
	int node_num;

	void Get_Largest_Subgraph();
	void Find_Start_Locate(int & first_node, int & second_node) const;
	void Rotate_To_Next(int & last_node, int & current_node);
	void Graph_Structure_Analyze();
public:
	void Prune(const Graph * graph_before_prune, Graph * graph_after_prune);
};

