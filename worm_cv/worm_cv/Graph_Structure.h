#pragma once
#include "CONST_PARA.h"

struct Graph_Structure_Node {
	int real_index;
	int degree;
	int edge_midway_1[SKELETONIZE::DEGREE_MAX];
	int edge_len[SKELETONIZE::DEGREE_MAX];
	int edge_midway_2[SKELETONIZE::DEGREE_MAX];
	int edge_end[SKELETONIZE::DEGREE_MAX];
};

class Graph_Structure {
private:
	Graph_Structure_Node *nodes;
	int *node_hash;
	int node_num;

	void Add_Edge_Oneway(int edge_start, int edge_end, int edge_len, int midway1, int midway2);
	void Delete_Edge_Oneway(int edge_start, int midway1);
public:
	Graph_Structure(int real_node_num, int structure_node_max);
	~Graph_Structure();
	void Check_Structure();
	void Add_Edge(int edge_start, int edge_end, int edge_len, int midway1, int midway2);
	void Delete_Edge(int edge_start, int edge_end, int midway1, int midway2);
	const Graph_Structure_Node * Get_Node_List() { return nodes; }
	int Get_Node_Num() { return node_num; }
	bool In_Edge_Check(int & last_node, int & current_node);
};