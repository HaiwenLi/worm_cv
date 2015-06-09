#include "stdafx.h"

using namespace std;

const static string js_act = "{\"action\":\"";
const static string js_para = "\",\"para\":\"";
const static string js_end = "\"},";

Multi_Points::Multi_Points(const Multi_Points & multi_points) :size(multi_points.size) {
	for (int i = 0; i < size; ++i)
		index[i] = multi_points[i];
}

Multi_Points & Multi_Points::operator=(const Multi_Points & multi_points) {
	size = multi_points.size;
	for (int i = 0; i < size; ++i)
		index[i] = multi_points.index[i];
	return *this;
}

ostream & operator<< (ostream & out, Multi_Points multi_points) {
	out << "size:" << multi_points.size << endl;
	for (int i = 0; i < multi_points.size; ++i)
		out << multi_points[i] << "\t";
	out << endl;
	return out;
}

void Bifurcate_Stack::Push(const Multi_Points & in_stack_points, int parent_index) {
	if (top >= SKELETONIZE::STORAGE_MAX)
		throw new Simple_Exception("Bifurcate_Stack:Stack Full Error!");
	parent_node[top] = parent_index;
	item[top] = in_stack_points;
	++top;
}

void Skeletonize::Add_Into_Graph(int parent_node) {
	for (int i = 0; i < current_item.size; ++i)
		point_mark[current_item[i]] = skeleton_graph->Get_Node_Num();
	skeleton_graph->Add_Node(candidate_points->Get_Center(current_item), parent_node);
#ifdef __OUTPUT_DEBUG_INFO
	js_out << js_act << "addNode" << js_end;
#endif
}

void Skeletonize::Current_Item_Bifurcate(int parent_node) {
	static int branch_stamp[10];
	static Multi_Points temp;
	int branch_num = candidate_points->Branch_Analize(current_item, branch_stamp);
	if (branch_num > 1) {
		for (int i = 0; i < branch_num - 1; ++i) {
			temp.size = 0;
			for (int j = 0; j < current_item.size; ++j)
				if (branch_stamp[j] == i)
					temp.Add(current_item[j]);
			stack.Push(temp, parent_node);
#ifdef __OUTPUT_DEBUG_INFO
			js_out << "{\"continue\":\"true\",\"action\":\"setStack" << js_para << candidate_points->getPointStr(temp) << js_end;
#endif
		}
		int size = current_item.size;

		current_item.size = 0;
		for (int j = 0; j < size; ++j)
			if (branch_stamp[j] == branch_num - 1)
				current_item.Add(current_item[j]);
	}
#ifdef __OUTPUT_DEBUG_INFO
	js_out << js_act << "select" << js_para << candidate_points->getPointStr(current_item) << js_end;
	if (branch_num > 1)
		js_out << js_act << "push" << js_end;
#endif
}

int Skeletonize::Find_Start_Point(const Multi_Points & item, double * pointer) const {
	Select_Minimum furthest_point(WORM::INF, -1);
	const int * point;
	for (int i = 0; i < item.size; ++i) {
		point = candidate_points->Get_Point(item[i]);
		furthest_point.Renew(-point[0] * pointer[0] - point[1] * pointer[1], i);
	}
	return furthest_point.Get_Min_Index();
}

void Skeletonize::Check_Used_Point(Multi_Points & item) const {
	for (int i = item.size - 1; i >= 0; --i) {
		if (point_mark[item[i]] >= 0) {
			--item.size;
			if (i < item.size)
				item[i] = item[item.size];
		}
	}
}

bool Skeletonize::Search_Further_Points(Multi_Points & last_item, int current_node) {
	double direction_vec[2], base_point[2];
	if (!skeleton_graph->Calc_End_Direction_Vec(current_node, direction_vec))
		return false;
	const int *base_point_temp;
	int base_index = Find_Start_Point(last_item, direction_vec);

	base_point_temp = candidate_points->Get_Point(last_item[base_index]);
	base_point[0] = base_point_temp[0] + SKELETONIZE::ANGLE_ERROR*direction_vec[0];
	base_point[1] = base_point_temp[1] + SKELETONIZE::ANGLE_ERROR*direction_vec[1];
	current_item = candidate_points->Query_Points_By_Pointer(base_point, direction_vec);
	if (current_item.size > 0 && point_mark[current_item[0]] >= 0) {
		skeleton_graph->Connect_Node(point_mark[current_item[0]], current_node);
		current_item.size = 0;
		return true;
	}
	return false;
}

bool Skeletonize::Search_Next_Points() {
	int current_node = skeleton_graph->Get_Node_Num() - 1;
	double direction_vec[2], base_point[2];
	const int * base_point_temp;
	Multi_Points last_item = current_item;
	current_item = candidate_points->Query_Points_Nearby(current_item);
	Check_Used_Point(current_item);
	if (current_item.size <= 0)
		Search_Further_Points(last_item, current_node);
	if (current_item.size <= 0)
		return false;
	else if (current_item.size == 1) {
#ifdef __OUTPUT_DEBUG_INFO
		js_out << js_act << "select" << js_para << candidate_points->getPointStr(current_item) << js_end;
#endif
		current_item = candidate_points->Query_Points_Nearby(current_item);
		Check_Used_Point(current_item);
#ifdef __OUTPUT_DEBUG_INFO
		if (current_item.size > 1)
			js_out << js_act << "reselect" << js_para << candidate_points->getPointStr(current_item) << js_end;
#endif
	}
	else
		Current_Item_Bifurcate(current_node);
	Add_Into_Graph(skeleton_graph->Get_Node_Num() - 1);
	return true;
}

bool Skeletonize::Check_Up_Stack() {
	bool node_used;
	while (stack.top > 0) {
		node_used = false;
		int stack_top = --stack.top;
#ifdef __OUTPUT_DEBUG_INFO
		js_out << js_act << "pop" << js_end;
#endif
		for (int i = 0; i < stack.item[stack_top].size; ++i)
			if (point_mark[stack.item[stack_top][i]] >= 0) {
				// 若结点已经使用则将其与母节点相连接
				int zi_node = point_mark[stack.item[stack_top][i]];
				const auto zi_graph_node = skeleton_graph->Get_Node(zi_node);
				auto connected = false;
				for (auto j = 0; j < zi_graph_node.degree; ++j)
					if (zi_graph_node.adjacent[j] == stack.parent_node[stack_top])
						connected = true;
				if (!connected) {
					skeleton_graph->Connect_Node(zi_node, stack.parent_node[stack_top]);
#ifdef __OUTPUT_DEBUG_INFO
					js_out << "{\"continue\":\"true\",\"action\":\"connectNode" << js_para << num2str(zi_node) + " " + num2str(stack.parent_node[stack_top]) << js_end;
					js_out << js_act << "unselect" << js_end;
#endif
				}
				node_used = true;
				break;
			}
		if (node_used == false) {// 若结点未使用过则表示栈中已找到所需元素
			current_item = stack.item[stack_top];
#ifdef __OUTPUT_DEBUG_INFO
			js_out << js_act << "setCurrent" << js_para << num2str(stack.parent_node[stack_top]) << js_end;;
#endif
			Add_Into_Graph(stack.parent_node[stack_top]);
			return true;
		}
	}
	return false;
}

void Skeletonize::Search_Unused_Points() {
	for (int i = 0; i < point_num; ++i)
		if (point_mark[i] < 0) {
			current_item = candidate_points->Query_Points_Nearby(static_cast<Multi_Points>(i));
			Check_Used_Point(current_item);
#ifdef __OUTPUT_DEBUG_INFO
			js_out << js_act << "select" << js_para << candidate_points->getPointStr(current_item) << js_end;
#endif
			Add_Into_Graph(-1);
			break;
		}
}

void Skeletonize::Connecting_End() {
	int end_node_num, *end_nodes, base_index;
	double direction_vec[2], base_point[2];
	bool end_node_changed = true;
	end_nodes = skeleton_graph->Get_End_Node();
	end_node_num = skeleton_graph->Get_End_Num();
	const int * base_point_temp;

	while (end_node_changed) {
		end_node_changed = false;
		for (int i = 0; i < end_node_num; ++i) {
			if (!skeleton_graph->Is_End_Node(end_nodes[i]) || end_nodes[i] < 0)
				continue;
			current_item.size = 0;
			for (int j = 0; j < point_num; ++j) {
				if (point_mark[j] == end_nodes[i])
					current_item.Add(j);
			}
			if (Search_Further_Points(current_item, end_nodes[i])) {
				end_node_changed = true;
				end_nodes[i] = point_mark[current_item[0]];
#ifdef __OUTPUT_DEBUG_INFO
				js_out << js_act << "connectNode" << js_para << end_nodes[i] << " " << point_mark[current_item[0]] << js_end;
#endif
			}
		}
	}
}

//创建骨架图，并对该图进行处理（Graph_Pruning）
void Skeletonize::Convert_To_Graph(const Candidate_Points * candidate_points, Graph * skeleton_graph, string pic_num_str) {
#ifdef __OUTPUT_DEBUG_INFO
	js_out.open("..\\..\\cache_data\\json\\json" + pic_num_str + ".js");
	js_out << "worm_json=[" << js_act << "init" << js_para << candidate_points->getWholeStr() << js_end;
#endif
	this->candidate_points = candidate_points;
	this->skeleton_graph = skeleton_graph;
	point_num = candidate_points->Get_Point_Num();
	point_mark = new int[point_num];
	for (int i = 0; i < point_num; ++i)
		point_mark[i] = -1;
	skeleton_graph->Reset();

	current_item = candidate_points->Query_Points_Nearby(Multi_Points(0));//将中心线候选点的第一个点设置为当前点，开始建立骨架图
#ifdef __OUTPUT_DEBUG_INFO
	js_out << js_act << "select" << js_para << candidate_points->getPointStr(static_cast<Multi_Points>(0)) << js_end;
#endif

	stack.top = 0;
	Add_Into_Graph(-1);

	int temp;
	while (current_item.size > 0) {
		temp = skeleton_graph->Get_Node_Num();
		if (Search_Next_Points())
			continue;
#ifdef __OUTPUT_DEBUG_INFO
		js_out << js_act << "removeCurrent" << js_end;
#endif
		if (Check_Up_Stack())
			continue;
		Search_Unused_Points();
	}
	Connecting_End();
#ifdef __OUTPUT_DEBUG_INFO
	js_out << "]";
	js_out.close();
#endif
	//skeleton_graph->Connect_Node(161, 140);
	//skeleton_graph->Connect_Node(108, 92);
	delete[] point_mark;
}