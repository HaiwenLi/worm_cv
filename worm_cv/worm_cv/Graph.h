#pragma once

#include "CONST_PARA.h"

struct Graph_Node{
	double center[2];
	int degree;
	int adjacent[SKELETONIZE::DEGREE_MAX];

	Graph_Node ():degree(0){
		center[0] = WORM::INF;
		center[1] = WORM::INF;
	}
	// 需在外部自行验证下标合法性
	int operator[](int i) const{
		return adjacent[i];
	}
	Graph_Node(const Graph_Node & node_from, bool copy_adjacent = true);
	Graph_Node & operator= (const Graph_Node & node);
	bool Select_Next(int& last_node, int& current_node) const {
		if (degree != 2)
			return false;
		auto next_node = (last_node == adjacent[0]) ? adjacent[1] : adjacent[0];
		last_node = current_node;
		current_node = next_node;
		return true;
	}
	int Get_Adjacent_Index(int adjacent_node) const;
};

class Graph{
private:
	int node_num;
	Graph_Node node[SKELETONIZE::POINT_NUM_MAX];
	
	mutable int end_node_num;
	mutable int end_node[SKELETONIZE::STORAGE_MAX];

	Graph(const Graph & graph){};
	Graph & operator=(const Graph & graph){};

public:
	Graph(){};
	void Reset(){
		node_num = 0;
	}
	void Connect_Node(int node_1, int node_2){
		node[node_1].adjacent[node[node_1].degree ++] = node_2;
		node[node_2].adjacent[node[node_2].degree ++] = node_1;
	}
	void Add_Node(const double center[2], int fu_node = -1){
		node[node_num].center[0] = center[0];
		node[node_num].center[1] = center[1];
		node[node_num].degree = 0;
		if (fu_node != -1){
			node[node_num].adjacent[node[node_num].degree ++] = fu_node;
			node[fu_node].adjacent[node[fu_node].degree ++] = node_num;
		}
		++ node_num;
	}
	int Get_Node_Num() const{
		return node_num;
	}
	int * Get_End_Node() const;
	int Get_End_Num() const{
		return end_node_num;
	}
	bool Is_End_Node(int node_index) const{
		return (node_index>=0 && node_index<node_num && node[node_index].degree==1);
	}
	const Graph_Node * Get_Node(int node_index) const{
		return node + node_index;
	}
	bool Calc_End_Direction_Vec(int end_node, double * derection_vec) const;
	static void persistence(void *obj_ptr, std::string out_file);
	static void anti_persistence(void *obj_ptr, std::string in_file);
};