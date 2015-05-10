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

class Owner_Mark{
private:
	typedef bool (Owner_Mark::* Fpointer)(int point_index) const;
	int owner_node[SKELETONIZE::POINT_NUM_MAX];
	int specific_index;
	bool check_All(int point_index) const{
		return (owner_node[point_index] < 0);
	}
	bool check_None(int point_index) const{
		return true;
	}
	bool check_Specific(int point_index) const{
		return (owner_node[point_index]!=specific_index);
	}
public:
	enum CHECK_MODE{CHECK_ALL=0, CHECK_NONE=1, CHECK_SPECIFIC=2};
	Owner_Mark();
	void reset();
	void set_Specific_index(int point_index){
		specific_index = point_index;
	}
	void Set_Owner_Node(Multi_Points points, int node_index);
	int Get_Owner_Node(int point_index) const;
	void Set_Check_Mode(enum CHECK_MODE check_mode, int specific_index = 0);
	Fpointer point_available;
};

class Skeletonize{
private:
	int point_num;
	Owner_Mark owner_mark;
	Multi_Points current_item;
	Bifurcate_Stack stack;
	std::ofstream js_out;

	Graph * skeleton_graph;
	const Candidate_Points * candidate_points;

	void Add_Into_Graph(int parent_node);
	void Current_Item_Bifurcate(int parent_node);
	bool Search_Next_Points();
	bool Check_Up_Stack();
	void Search_Unused_Points();
	void Connecting_End();
public:
	void Convert_To_Graph(const Candidate_Points * candidate_center_points, Graph * skeleton_graph);
};
