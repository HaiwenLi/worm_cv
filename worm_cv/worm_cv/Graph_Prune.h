#pragma once

#include "CONST_PARA.h"

class Graph;

class Graph_Prune{
private:
	bool node_available[SKELETONIZE::POINT_NUM_MAX];
	const Graph * unpruned_graph;
	Graph * pruned_graph;

	int Find_Leftmost_Node() const;
	// 返回序号为base_node的结点从direction所表示方向开始，顺时针旋转找到的第一个相邻结点的序号
	int Rotate_To_Next(int base_node, double direction) const;
	// 边缘探测函数，运行该函数将去除图的内部结点、内部边，并调整图的存储
	void Edge_Search();
public:
	void Prune(const Graph * unpruned_graph, Graph * pruned_graph);
};

