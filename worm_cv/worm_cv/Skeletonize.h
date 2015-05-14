#pragma once

#include "Graph.h"

class Graph;
class Subgraph_Count;

struct Bifurcate_Stack{
	int top;
	int parent_node[SKELETONIZE::STORAGE_MAX];
	Multi_Points item[SKELETONIZE::STORAGE_MAX];

	Bifurcate_Stack():top(0){};
	void Push(const Multi_Points & in_stack_points, int parent_index);
};

class Skeletonize{
private:
	int point_num;
	Multi_Points current_item;
	int * point_mark;
	Bifurcate_Stack stack;
	std::ofstream js_out;

	Graph * skeleton_graph;
	const Candidate_Points * candidate_points;

	int Find_Start_Point(const Multi_Points & item, double * pointer) const;
	bool Search_Further_Points(Multi_Points & item, int current_node);
	void Check_Used_Point(Multi_Points & item) const;
	void Add_Into_Graph(int parent_node);
	void Current_Item_Bifurcate(int parent_node);
	bool Search_Next_Points();
	bool Check_Up_Stack();
	void Search_Unused_Points();
	void Connecting_End();
public:
	void Convert_To_Graph(const Candidate_Points * candidate_center_points, Graph * skeleton_graph, std::string pic_num_str);
};
