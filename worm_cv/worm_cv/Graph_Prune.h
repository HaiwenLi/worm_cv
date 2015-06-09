#pragma once
#include"Graph.h"
#include"Graph_Structure.h"


class Graph_Prune {
private:
	const Graph * before_prune;
	Graph_Structure * graph_structure;
	bool * node_available;
	int node_num;
	int structure_node_num;
	const Graph_Structure_Node * structure_node_list;

	void Get_Largest_Subgraph();
	void Rotate_To_Next(int & last_node, int & current_node) const;
	bool Select_Next(int & last_node, int & current_node)const;
	void Start_Node_Locate(int & first_node, int & second_node) const;
	void Graph_Structure_Analyze(int & first_node, int & second_node);

	void Delete_Short_Route();
	bool Shotcut_Prune();
	bool Branch_Prune();
	void Structure_Node_Statistic(int& special_node_num, int& self_ring_count, const Graph_Structure_Node* special_node[2]);
	void Self_Ring_Prune(int& bifurcate_node_num, const Graph_Structure_Node* special_node[2]);
public:
	std::vector<int> Prune(const Graph * graph_before_prune, Centerline & last_backbone, double worm_full_width, bool is_first_pic);
};

