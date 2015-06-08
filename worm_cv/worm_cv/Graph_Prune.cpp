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

void Graph_Prune::Rotate_To_Next(int & last_node, int & current_node) const {
	const auto & current_graph_node = before_prune->Get_Node(current_node);
	const auto & last_graph_node = before_prune->Get_Node(last_node);
	auto direction = atan2(last_graph_node.center[0] - current_graph_node.center[0],
		last_graph_node.center[1] - current_graph_node.center[1]) + SKELETONIZE::ANGLE_ERROR;
	Select_Minimum adjacent_select(WORM::INF, -1);
	double angle_temp;
	int node_to;
	const double * temp_center;
	for (auto i = 0; i < current_graph_node.degree; ++i) {
		node_to = current_graph_node.adjacent[i];
		if (!node_available[node_to]) continue;
		temp_center = before_prune->Get_Center(node_to);
		// angle_temp 表示从direction到base_node指向node_to的方向的顺时针转角，范围在0-2pi
		angle_temp = atan2(temp_center[0] - current_graph_node.center[0],
			temp_center[1] - current_graph_node.center[1]) - direction;
		while (angle_temp < 0)
			angle_temp += 2 * WORM::PI;
		adjacent_select.Renew(angle_temp, i);
	}
	last_node = current_node;
	current_node = current_graph_node.adjacent[adjacent_select.Get_Min_Index()];
}

bool Graph_Prune::Select_Next(int& last_node, int& current_node) const {
	const auto & curren_graph_node = before_prune->Get_Node(current_node);
	const auto & last_graph_node = before_prune->Get_Node(last_node);
	auto next_node = -1;
	for (auto i = 0; i < curren_graph_node.degree; ++i) {
		if (!node_available[curren_graph_node.adjacent[i]] || curren_graph_node.adjacent[i] == last_node)
			continue;
		if (next_node >= 0)
			return false;
		else
			next_node = curren_graph_node.adjacent[i];
	}
	if (next_node < 0)
		return false;
	last_node = current_node;
	current_node = next_node;
	return true;
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
	auto edge_num = 1;
	while (Select_Next(first_node, second_node)) {
		if (++edge_num > node_num)
			throw new Simple_Exception("Circle Error!");
	}
	Rotate_To_Next(first_node, second_node);
}

void Graph_Prune::Graph_Structure_Analyze() {
	int last_node, current_node;
	int edge_start, edge_second;
	int edge_num;
	last_node = first_node;
	current_node = second_node;
	while (true) {
		edge_start = last_node;
		edge_second = current_node;
		edge_num = 1;
		while (Select_Next(last_node, current_node))
			++edge_num;
		graph_structure->Add_Edge(edge_start, current_node, edge_num, edge_second, last_node);
		do {
			Rotate_To_Next(last_node, current_node);
			if (last_node == first_node && current_node == second_node)
				return;
		} while (graph_structure->Move_To_Other_End(last_node, current_node));
	}
}

void Graph_Prune::Delete_Edge(int edge_start, int edge_end, int midway1, int midway2, bool branch) {
	while ((edge_start == first_node && midway1 == second_node) || (edge_end == first_node && midway2 == second_node)) {
		graph_structure->Move_To_Other_End(first_node, second_node);
		Rotate_To_Next(first_node, second_node);
	}
	if (!(edge_start == midway2 || branch))
		node_available[midway2] = false;
	if (edge_end != midway1)
		node_available[midway1] = false;
	graph_structure->Delete_Edge(edge_start, edge_end, midway1, midway2);
}

void Graph_Prune::Delete_Short_Route() {
	for (auto i = 0; i < structure_node_num; ++i) {
		const auto & structure_node = structure_node_list[i];
		for (auto j = 0; j < structure_node.degree - 1; ++j) {
			if (structure_node.real_index == structure_node.edge_end[j] && structure_node.edge_len[j] <= 4)
				Delete_Edge(structure_node.real_index, structure_node.real_index, structure_node.edge_midway_1[j], structure_node.edge_midway_2[j]);
		}
		if (structure_node.degree == 1 && structure_node.edge_len[0] <= 2)
			Delete_Edge(structure_node.real_index, structure_node.edge_end[0], structure_node.edge_midway_1[0], structure_node.edge_midway_2[0], true);
	}
	graph_structure->Check_Structure(first_node, second_node);
}

bool Graph_Prune::Shotcut_Prune() {
	auto changed = false;
	int delete_index;
	for (auto i = 0; i < structure_node_num; ++i) {
		const auto & structure_node = structure_node_list[i];
		for (auto j = 0; j < structure_node.degree - 1; ++j) {
			if (structure_node.edge_end[j] == structure_node.real_index)
				continue;
			for (auto k = j + 1; k < structure_node.degree; ++k)
				if (structure_node.edge_end[j] == structure_node.edge_end[k]) {
					changed = true;
					delete_index = structure_node.edge_len[j]>structure_node.edge_len[k] ? k : j;
					Delete_Edge(structure_node.real_index, structure_node.edge_end[delete_index],
						structure_node.edge_midway_1[delete_index], structure_node.edge_midway_2[delete_index]);
				}
		}
	}
	if (changed)
		graph_structure->Check_Structure(first_node, second_node);
	return changed;
}

bool Graph_Prune::Branch_Prune() {
	auto changed = false;
	auto branch_index = new int[structure_node_num*SKELETONIZE::DEGREE_MAX];
	auto branch_len = new int[structure_node_num*SKELETONIZE::DEGREE_MAX];
	auto branch_num = 0;
	// 寻找所有的分支与自环
	for (auto i = 0; i < structure_node_num; ++i) {
		const auto & structure_node = structure_node_list[i];
		if (structure_node.degree == 1) {
			branch_index[branch_num] = i*SKELETONIZE::DEGREE_MAX;
			branch_len[branch_num] = structure_node.edge_len[0];
			++branch_num;
		}
		else for (auto j = 0; j < structure_node.degree; ++j) {
			if (structure_node.edge_end[j] == structure_node.real_index &&
				structure_node.edge_midway_1[j] < structure_node.edge_midway_2[j]) {
				branch_index[branch_num] = i*SKELETONIZE::DEGREE_MAX + j;
				branch_len[branch_num] = structure_node.edge_len[j] >> 1;
				++branch_num;
			}
		}
	}
	if (branch_num > 2) {
		changed = true;
		// 找到最长两个分支
		Select_Minimum longest_branch(WORM::INF, -1);
		Select_Minimum second_longest_branch(WORM::INF, -1);
		for (auto i = 0; i < branch_num; ++i)
			longest_branch.Renew(-branch_len[i], i);
		for (auto i = 0; i < branch_num; ++i)
			if (i != longest_branch.Get_Min_Index())
				second_longest_branch.Renew(-branch_len[i], i);
		// 删除最长两个分支之外的分支
		for (auto i = 0; i < branch_num; ++i)
			if (!(i == longest_branch.Get_Min_Index() || i == second_longest_branch.Get_Min_Index())) {
				const auto & structure_node_delete = structure_node_list[branch_index[i] / SKELETONIZE::DEGREE_MAX];
				auto edge_delete = branch_index[i] % 5;
				Delete_Edge(structure_node_delete.real_index, structure_node_delete.edge_end[edge_delete],
					structure_node_delete.edge_midway_1[edge_delete], structure_node_delete.edge_midway_2[edge_delete]);
			}
		graph_structure->Check_Structure(first_node, second_node);
	}
	delete[] branch_index;
	delete[] branch_len;
	return changed;
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
	Find_Start_Locate(first_node, second_node);

	structure_node_num = 2;
	for (auto i = 0; i < node_num; ++i)
		if (node_available[i] && before_prune->Get_Node(i).degree != 2)
			++structure_node_num;
	graph_structure = new Graph_Structure(node_num, structure_node_num);
	structure_node_list = graph_structure->Get_Node_List();

	Graph_Structure_Analyze();
	graph_structure->Check_Structure(first_node, second_node);
	Delete_Short_Route();
	while (Shotcut_Prune() || Branch_Prune());

	delete[] node_available;
	delete graph_structure;
}
