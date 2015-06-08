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
	int structure_node_num;
	const Graph_Structure_Node * structure_node_list;
	int first_node;
	int second_node;

	void Get_Largest_Subgraph();
	void Rotate_To_Next(int & last_node, int & current_node) const;
	bool Select_Next(int & last_node, int & current_node)const;
	void Find_Start_Locate(int & first_node, int & second_node) const;
	void Graph_Structure_Analyze();

	void Delete_Edge(int edge_start, int edge_end, int midway1, int midway2, bool branch=false);
	void Delete_Short_Route();
	bool Shotcut_Prune();
	bool Branch_Prune();
public:
	void Prune(const Graph * graph_before_prune, Graph * graph_after_prune);
};

