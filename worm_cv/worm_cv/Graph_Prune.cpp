#include "stdafx.h"

using namespace std;

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

void Graph_Prune::Find_Start_Locate(int & first_node, int & second_node) const {
	Select_Minimum find_leftmost_point(WORM::INF, -1);
	Select_Minimum second_select(WORM::INF, -1);
	for (auto i = 0; i < node_num; ++i) {
		if (node_available[i]) 
			find_leftmost_point.Renew(before_prune->Get_Center(i)[0], i);
	}
	first_node = find_leftmost_point.Get_Min_Index();
	const auto leftmost_node = before_prune->Get_Node(first_node);
	int node_to;
	for (auto i = 0; i < leftmost_node.degree; ++i) {
		node_to = leftmost_node.adjacent[i];
		if (!node_available[node_to]) continue;
		second_select.Renew(atan2(before_prune->Get_Center(node_to)[0] - leftmost_node.center[0],
			before_prune->Get_Center(node_to)[1] - leftmost_node.center[1]), i);
	}
	second_node = leftmost_node.adjacent[second_select.Get_Min_Index()];
}

void Graph_Prune::Rotate_To_Next(int & last_node, int & current_node) {
	const auto & current_graph_node = before_prune->Get_Node(current_node);
	const auto & last_graph_node = before_prune->Get_Node(last_node);
	auto direction = atan2(last_graph_node.center[0] - current_graph_node.center[0],
		last_graph_node.center[1] - current_graph_node.center[1]) + SKELETONIZE::ANGLE_ERROR;
	Select_Minimum adjacent_select(WORM::INF, -1);
	double angle_temp;
	int node_to;
	for (auto i = 0; i < current_graph_node.degree; ++i) {
		node_to = current_graph_node.adjacent[i];
		if (!node_available[node_to]) continue;
		// angle_temp 表示从direction到base_node指向node_to的方向的顺时针转角，范围在0-2pi
		angle_temp = atan2(before_prune->Get_Center(node_to)[0] - current_graph_node.center[0],
			before_prune->Get_Center(node_to)[1] - current_graph_node.center[1]) - direction;
		while (angle_temp < 0)
			angle_temp += 2 * WORM::PI;
		adjacent_select.Renew(angle_temp, i);
	}
	last_node = current_node;
	current_node = current_graph_node.adjacent[adjacent_select.Get_Min_Index()];
}

void Graph_Prune::Graph_Structure_Analyze() {
	int first_node, second_node;
	int last_node, current_node;
	int edge_start, edge_second;
	Find_Start_Locate(first_node, second_node);
	last_node = first_node;
	current_node = second_node;
	edge_start = first_node;
	edge_second = second_node;
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

	auto structure_node_num = 0;
	for (auto i = 0; i < node_num; ++i)
		if (node_available[i] && before_prune->Get_Node(i).degree != 2)
			++structure_node_num;
	graph_structure = new Graph_Structure(node_num, structure_node_num + 2);

	int start_node, second_node;
	Find_Start_Locate(start_node, second_node);
	cout << "new leftmost:" << start_node << "\t" << second_node << endl;
	delete[] node_available;
	delete graph_structure;
}
