#include "stdafx.h"

using namespace std;

const static string js_act = "{\"action\":\"";
const static string js_para = "\",\"para\":\"";
const static string js_end = "\"},";

void Bifurcate_Stack::Push(const vector<int> & in_stack_points, int parent_index) {
	if (top >= SKELETONIZE::STORAGE_MAX)
		throw new Simple_Exception("Bifurcate_Stack:Stack Full Error!");
	parent_node[top] = parent_index;
	item[top] = in_stack_points;
	++top;
}

void Graph_Builder::Check_Connectivity(int parent_node) {
	static int branch_stamp[10];
	static vector<int> temp;
	int branch_num = candidate_points->Branch_Analize(selected_points, branch_stamp);
	if (branch_num > 1) {
		for (int i = 0; i < branch_num; ++i) {
			temp.clear();
			for (int j = 0; j < selected_points.size(); ++j)
				if (branch_stamp[j] == i)
					temp.push_back(selected_points[j]);
			stack.Push(temp, parent_node);
		}
		selected_points = temp;
	}
}

void Graph_Builder::Get_Unused_Points(const std::vector<int>& nearby_points, std::vector<int> & unused_points) const {
	unused_points.clear();
	for (auto itor = nearby_points.begin(); itor != nearby_points.end(); ++itor)
		if (point_mark[*itor] < 0)
			unused_points.push_back(*itor);
}

void Graph_Builder::Search_Further_Points(vector<int> & points_in_current_node, int current_node_index) {
	double direction_vec[2];
	auto current_node = skeleton_graph->Get_Node(current_node_index);
	if (current_node->degree != 1)
		return;
	auto last_node = skeleton_graph->Get_Node(current_node->adjacent[0]);
	for (auto i = 0; i < 2; ++i)
		direction_vec[i] = current_node->center[i] - last_node->center[i];
	auto direction_norm_square = direction_vec[0] * direction_vec[0] + direction_vec[1] * direction_vec[1];
	if (direction_norm_square == 0)
		return;
	// find base point by projection
	const int * temp_center;
	double projection_len;
	double base_point[2] = { current_node->center[0], current_node->center[1] };
	for (auto itor = points_in_current_node.begin(); itor < points_in_current_node.end(); ++itor) {
		temp_center = candidate_points->Get_Point(*itor);
		projection_len = direction_vec[0] * (temp_center[0] - base_point[0]) + direction_vec[1] * (temp_center[1] - base_point[1]);
		if (projection_len > 0) {
			base_point[0] += direction_vec[0] * projection_len / direction_norm_square;
			base_point[1] += direction_vec[1] * projection_len / direction_norm_square;
		}
	}
	base_point[0] += SKELETONIZE::ANGLE_ERROR*direction_vec[0];
	base_point[1] += SKELETONIZE::ANGLE_ERROR*direction_vec[1];
	// check used point
	auto single_node = candidate_points->Query_Points_By_Pointer(base_point, direction_vec);
	if (single_node >= 0)
		selected_points.push_back(single_node);
	if (!selected_points.empty() && point_mark[selected_points[0]] >= 0) {
		skeleton_graph->Connect_Node(point_mark[selected_points[0]], current_node_index);
		selected_points.clear();
	}
}

void Graph_Builder::Search_Next_Points() {
	current_node = skeleton_graph->Get_Node_Num() - 1;
	auto points_in_current_node = selected_points;
	static vector<int> nearby_points;

	candidate_points->Query_Points_Nearby(selected_points, nearby_points);
	Get_Unused_Points(nearby_points, selected_points);
	if (!selected_points.empty())
		Check_Connectivity(current_node);
	if (selected_points.empty())
		Search_Further_Points(points_in_current_node, current_node);
	// try to find unused points in stack
	if (selected_points.empty()) {
		while (stack.top > 0) {
			--stack.top;
			if (point_mark[stack.item[stack.top][0]] < 0) {
				selected_points = stack.item[stack.top];
				current_node = stack.parent_node[stack.top];
			}
		}
	}
	// try to find an unused point
	if (selected_points.empty()) {
		for (auto i = 0; i < candidate_points->Get_Point_Num(); ++i)
			if (point_mark[i] < 0) {
				selected_points.push_back(i);
				break;
			}
		current_node = -1;
	}
	// try to extend single point 
	if (selected_points.size() == 1) {
		candidate_points->Query_Points_Nearby(selected_points, nearby_points);
		Get_Unused_Points(nearby_points, selected_points);
	}
	// add selected points into graph
	if (!selected_points.empty()) {
		for (auto itor = selected_points.begin(); itor < selected_points.end(); ++itor)
			point_mark[*itor] = skeleton_graph->Get_Node_Num();
		skeleton_graph->Add_Node(candidate_points->Get_Center(selected_points), current_node);
	}
}

void Graph_Builder::Connecting_End() {
	for (auto i = 0; i < skeleton_graph->Get_Node_Num(); ++i) {
		auto end_node_index = i;
		while (end_node_index>=0 && skeleton_graph->Get_Node(end_node_index)->degree == 1) {
			selected_points.clear();
			for (auto j = 0; j < candidate_points->Get_Point_Num(); ++j)
				if (point_mark[j] == end_node_index)
					selected_points.push_back(j);
			Search_Further_Points(selected_points, end_node_index);
			if (!selected_points.empty())
				end_node_index = point_mark[selected_points.at(0)];
			else end_node_index = -1;
		}
	}
}

void Graph_Builder::Convert_To_Graph(const Candidate_Points * candidate_points, Graph * skeleton_graph, string pic_num_str) {
	this->candidate_points = candidate_points;
	this->skeleton_graph = skeleton_graph;
	skeleton_graph->Reset();
	auto point_num = candidate_points->Get_Point_Num();
	point_mark = new int[point_num];
	for (auto i = 0; i < point_num; ++i)
		point_mark[i] = -1;
	stack.top = 0;
	current_node = -1;
	selected_points.clear();
	selected_points.reserve(SKELETONIZE::NODE_SIZE_MAX);
	selected_points.push_back(0);
	while (!selected_points.empty())
		Search_Next_Points();
	Connecting_End();
	delete[] point_mark;
}