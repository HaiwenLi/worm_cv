#include "stdafx.h"
#include "Graph_Prune.h"

using namespace std;

Graph_Structure::Graph_Structure(int node_num, int key_node_num) :node_num(node_num), key_node_num(0) {
	edges = new Graph_Edge[key_node_num][SKELETONIZE::DEGREE_MAX];
	key_node_degree = new int[key_node_num];
	hash_index = new int[node_num];
}

Graph_Structure::~Graph_Structure() {
	if (edges != nullptr)
		delete[] edges;
	if (key_node_degree != nullptr)
		delete[] key_node_degree;
	if (hash_index != nullptr)
		delete[] hash_index;
}

void Graph_Structure::Check_Structure_Node(int node_index) {
	auto key_node_index = hash_index[node_index];
	if (key_node_degree[key_node_index] != 2)
		return;
	for (auto i = 0; i < key_node_degree[key_node_index]; ++i)
		if (edges[key_node_index][i].start_node == edges[key_node_index][i].end_node)
			return;
	auto new_edge_start = edges[key_node_index][0].end_node;
	auto new_edge_second = edges[key_node_index][0].last_second_node;
	auto new_edge_last_second = edges[key_node_index][1].last_second_node;
	auto new_edge_end = edges[key_node_index][1].end_node;
	auto new_edge_length = edges[key_node_index][0].length + edges[key_node_index][1].length;
	if (new_edge_start == new_edge_end){
	}
	key_node_degree[key_node_index] = 0;
}

void Graph_Prune::Get_Largest_Subgraph() {
	int *subgraph_mark, subgraph_num = 0;
	int * node_stack, stack_top = 0;
	//初始化
	subgraph_mark = new int[node_num];
	node_stack = new int[node_num];
	for (auto i = 0; i < node_num; ++i) {
		subgraph_mark[i] = -1;
	}
	//深搜每个分支
	int current_index;
	Graph_Node current_node;
	for (auto i = 0; i < node_num; ++i) {
		if (subgraph_mark[i] < 0) {
			node_stack[stack_top++] = i;
		}
		while (stack_top > 0) {
			current_index = node_stack[--stack_top];
			subgraph_mark[current_index] = subgraph_num;
			current_node = before_prune->Get_Node(current_index);
			for (auto j = 0; j < current_node.degree; ++j) {
				if (subgraph_mark[current_node.adjacent[j]] < 0) {
					node_stack[stack_top++] = current_node.adjacent[j];
				}
			}
		}
		++subgraph_num;
	}
	// 计算分支大小
	auto subgraph_count = new int[subgraph_num];
	for (auto i = 0; i < subgraph_num; ++i) {
		subgraph_count[i] = 0;
	}
	for (auto i = 0; i < node_num; ++i) {
		++subgraph_count[subgraph_mark[i]];
	}
	// 选择最大分支
	Select_Minimum find_largest_subgraph(-subgraph_count[0], 0);
	for (auto i = 1; i < subgraph_num; ++i) {
		find_largest_subgraph.Renew(-subgraph_count[i], i);
	}
	auto largest_subgraph = find_largest_subgraph.Get_Min_Index();
	for (auto i = 0; i < node_num; ++i) {
		node_available[i] = (subgraph_mark[i] == largest_subgraph);
	}
	delete[] subgraph_count;
	delete[] subgraph_mark;
	delete[] node_stack;
}

int Graph_Prune::Find_Leftmost_Point() {
	Select_Minimum find_leftmost_point(WORM::INF, -1);
	for (auto i = 0; i < node_num; ++i) {
		if (node_available[i]) {
			find_leftmost_point.Renew(before_prune->Get_Center(i)[0], i);
		}
	}
	return find_leftmost_point.Get_Min_Index();
}

void Graph_Prune::Prune(const Graph * graph_before_prune, Graph * graph_after_prune) {
	before_prune = graph_before_prune;
	after_prune = graph_after_prune;
	after_prune->Reset();
	node_num = before_prune->Get_Node_Num();
	node_available = new bool[node_num];
	for (auto i = 0; i < node_num; ++i) {
		node_available[i] = true;
	}
	Get_Largest_Subgraph();
	cout << "new leftmost:" << Find_Leftmost_Point() << endl;
	delete[] node_available;
}
