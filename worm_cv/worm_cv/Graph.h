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
	void Find_Available_Adjacents(Graph_Node & next_node, int * node_available) const;
	int Find_Another_Adjacent(int last_node) const{
		if (degree != 2)
			std::cout << "Warning: This method should only be used by degree-2 node!"<<std::endl;
		return adjacent[0] == last_node?adjacent[1]:adjacent[0];//一般处理度为2的节点
	}
	int Adjacent_Index_Locate(int adjacent_node) const;
};

class Subgraph_Count{
private:
	int node_num;
	int subgraph_num;
	int subgraph_node_count[SKELETONIZE::SUBGRAPH_MAX];
	int node_in_which_subgraph[SKELETONIZE::POINT_NUM_MAX];
public:
	Subgraph_Count():node_num(0), subgraph_num(1){
		subgraph_node_count[0] = 0;
	};
	Subgraph_Count(const Subgraph_Count & subgraph_count);
	Subgraph_Count & operator=(const Subgraph_Count & subgraph_count);
	void Reset(){
		node_num = 0;
		subgraph_num = 1;
		subgraph_node_count[0] = 0;
	}
	void Add_Node(){
		++ subgraph_node_count[subgraph_num - 1];
		node_in_which_subgraph[node_num ++] = subgraph_num - 1;
	}
	void Add_Subgraph(){
		if (subgraph_num ++ >= SKELETONIZE::SUBGRAPH_MAX)
			throw new Simple_Exception("Subgraph Number Exceed Error!");
		subgraph_node_count[subgraph_num - 1] = 0;
	}
	void Connect_subgraph(int node_1, int node_2);
	// 若序号为i的结点不在最大连通分支内，则将point_in_which_node[i]标记为-1
	void Select_Largest_subgraph(bool * Node_Saved);
};

class Graph{
private:
	int node_num;
	Graph_Node node[SKELETONIZE::POINT_NUM_MAX];
	bool node_available[SKELETONIZE::POINT_NUM_MAX];
	Subgraph_Count subgraph_count;
	
	mutable int end_node_num;
	mutable int end_node[SKELETONIZE::STORAGE_MAX];

	Graph(const Graph & graph){};
	Graph & operator=(const Graph & graph){};

	int Find_Leftmost_Node() const;
	// 返回序号为base_node的结点从direction所表示方向开始，顺时针旋转找到的第一个相邻结点的序号
	int Rotate_To_Next(int base_node, double direction) const;
public:
	Graph(){};
	~Graph() {};
	void Reset(){
		node_num = 0;
		subgraph_count.Reset();
	}
	void Connect_Node(int node_1, int node_2){
		node[node_1].adjacent[node[node_1].degree ++] = node_2;
		node[node_2].adjacent[node[node_2].degree ++] = node_1;
		subgraph_count.Connect_subgraph(node_1, node_2);
	}
	void Add_Node(const double center[2], int fu_node = -1){
		if (fu_node == -1 && node_num > 0)
			subgraph_count.Add_Subgraph();
		node[node_num].center[0] = center[0];
		node[node_num].center[1] = center[1];
		node[node_num].degree = 0;
		subgraph_count.Add_Node();
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
	const double * Get_Center(int node_index) const{
		return node[node_index].center;
	}
	const Graph_Node & Get_Node(int node_index) const{
		return node[node_index];
	}
	void Delete_Node(int node_index);
	bool Calc_End_Direction_Vec(int end_node, double * derection_vec) const;
	// 边缘探测函数，运行该函数将去除图的内部结点、内部边，并调整图的存储
	void Edge_Search(Graph & pruned_graph);
	static void Graph::persistence(int *obj_ptr, std::string file_name);
};