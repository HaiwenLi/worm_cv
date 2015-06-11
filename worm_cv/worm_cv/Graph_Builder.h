#pragma once

#include "Graph.h"

class Graph;
class Subgraph_Count;

struct Bifurcate_Stack{
	int top;
	int parent_node[SKELETONIZE::STORAGE_MAX];
	std::vector<int> item[SKELETONIZE::STORAGE_MAX];

	Bifurcate_Stack():top(0){};
	void Push(const std::vector<int> & in_stack_points, int parent_index);
};

class Graph_Builder{
private:
	std::vector<int> selected_points;
	int current_node;
	int * point_mark;
	Bifurcate_Stack stack;

	Graph * skeleton_graph;
	const Candidate_Points * candidate_points;

	void Get_Unused_Points(const std::vector<int>& nearby_points, std::vector<int> & unused_points) const;
	void Search_Further_Points(std::vector<int> & item, int current_node);
	void Check_Connectivity(int parent_node);
	void Search_Next_Points();
	void Connecting_End();
public:
	void Convert_To_Graph(const Candidate_Points * candidate_center_points, Graph * skeleton_graph, std::string pic_num_str);
};
