#include "stdafx.h"

using namespace std;

Graph_Node & Graph_Node::operator=(const Graph_Node & node){
	degree = node.degree;
	for (int i = 0; i < degree; i++){
		adjacent[i] = node.adjacent[i];
	}
	center[0] = node.center[0];
	center[1] = node.center[1];
	return * this;
}

Graph_Node::Graph_Node(const Graph_Node & node_from, bool copy_adjacent){
	center[0] = node_from.center[0];
	center[1] = node_from.center[1];
	if (copy_adjacent){
		degree = node_from.degree;
		for (int i = 0;i < degree;++ i)
			adjacent[i] = node_from.adjacent[i];
	}
	else
		degree = 0;
}

int Graph_Node::Adjacent_Index_Locate(int adjacent_node) const{
	for (int i = 0;i < degree;++ i)
		if (adjacent[i] == adjacent_node)
			return i;
	return -1;
}

void Graph_Node::Find_Available_Adjacents(Graph_Node & available_adjacents, int * node_unavailable) const{
	available_adjacents.degree = 0;
	for (int i = 0; i < degree; i++)
		if (node_unavailable[adjacent[i]] < 0)
			available_adjacents.adjacent[available_adjacents.degree ++] = adjacent[i];
}

Subgraph_Count::Subgraph_Count(const Subgraph_Count & subgraph_count):
	node_num(subgraph_count.node_num), subgraph_num(subgraph_count.subgraph_num){
		for (int i = 0;i < node_num;++ i)
			node_in_which_subgraph[i] = subgraph_count.node_in_which_subgraph[i];
		for (int i = 0;i < subgraph_num;++ i)
			subgraph_node_count[i] = subgraph_count.subgraph_node_count[i];
}
Subgraph_Count & Subgraph_Count::operator=(const Subgraph_Count & subgraph_count){
	node_num = subgraph_count.node_num;
	subgraph_num = subgraph_count.subgraph_num;
	for (int i = 0;i < node_num;++ i)
		node_in_which_subgraph[i] = subgraph_count.node_in_which_subgraph[i];
	for (int i = 0;i < subgraph_num;++ i)
		subgraph_node_count[i] = subgraph_count.subgraph_node_count[i];
	return *this;
}
//连接节点node_1和node_2所在的子图
void Subgraph_Count::Connect_subgraph(int node_1, int node_2){
	if (node_1 < 0 || node_1 >= node_num || node_2 < 0 || node_2 >= node_num)
		throw new Simple_Exception("Subgraph Count: Connect_subgraph error! Index out of bound!");
	int subgraph_1 = node_in_which_subgraph[node_1];
	int subgraph_2 = node_in_which_subgraph[node_2];
	if (subgraph_1 == subgraph_2) return;
	for (int i = 0;i < node_num;++ i)
		if (node_in_which_subgraph[i] == subgraph_2)
			node_in_which_subgraph[i] = subgraph_1;
	subgraph_node_count[subgraph_1] += subgraph_node_count[subgraph_2];
	subgraph_node_count[subgraph_2] = 0;
}
//在所有子图中寻找最大的连通分支
void Subgraph_Count::Select_Largest_subgraph(bool * Node_Saved){
	int largest_subgraph = 0;
	// 找到最大的连通分支
	for (int i = 0;i < subgraph_num;++ i)
		if (subgraph_node_count[i] > subgraph_node_count[largest_subgraph])
			largest_subgraph = i;
	// 将最大连通分支之外的点标记为-1
	for (int i = 0;i < node_num;++ i)
		Node_Saved[i] = (node_in_which_subgraph[i] == largest_subgraph);
}

int Graph::Find_Leftmost_Node() const{
	Select_Minimum select_minimum_node(WORM::INF, -1);
	for (int i = 0;i < node_num; ++i)
		if (node_available[i])
			select_minimum_node.Renew(node[i].center[0], i);
	return select_minimum_node.Get_Min_Index();
}

void Graph::Delete_Node(int node_index){
	node_available[node_index] = false;
	for (int i = 0;i < node[node_index].degree;++ i){
		int adj_node = node[node_index].adjacent[i];
		int loc = node[adj_node].Adjacent_Index_Locate(node_index);
		if (loc == -1)
			throw new Simple_Exception("Graph Build Error: One-way Adjacent!");
		else{
			-- node[adj_node].degree;
			node[adj_node].adjacent[loc] = node[adj_node].adjacent[node[adj_node].degree];
		}
	}
	node[node_index].degree = 0;
}

//计算从base_node起与direction方向夹角最小的节点
int Graph::Rotate_To_Next(int base_node, double direction) const{
	Select_Minimum adjacent_select(WORM::INF, 0);
	double angle_temp;
	int node_to;
	for (int i = 0;i < node[base_node].degree;++ i){
		if (!node_available[node[base_node].adjacent[i]]) continue;
		node_to = node[base_node].adjacent[i];
		// angle_temp 表示从结点base_node指向结点node_to的方向与direction表示的方向之间的夹角
		angle_temp = atan2(node[node_to].center[0] - node[base_node].center[0], 
			node[node_to].center[1] - node[base_node].center[1]) - direction;
		while (angle_temp < 0) 
			angle_temp += 2 * WORM::PI;
		adjacent_select.Renew(angle_temp, i);
	}
	return node[base_node].adjacent[adjacent_select.Get_Min_Index()];
}
//对得到的骨架图进行处理
void Graph::Edge_Search(Graph & pruned_graph){
	static int new_index[WORM::IMAGE_SIZE];
	for (int i = 0; i < node_num; i++)
		new_index[i] = -1;
	// 选择最大的连通分支
	subgraph_count.Select_Largest_subgraph(node_available);
	// 去除长度为1的小分支
	for (int i = 0;i < node_num;++ i)
		if (node[i].degree == 1 && node[node[i].adjacent[0]].degree > 2)
			Delete_Node(i);

	int last_node = Find_Leftmost_Node();
	int current_node = Rotate_To_Next(last_node, 0);
	int next_node;
	int stack[SKELETONIZE::STORAGE_MAX][2], top = 0;
	// 初始化过程
	pruned_graph.Reset();
	pruned_graph.Add_Node(node[last_node].center, -1);
	new_index[last_node] = 0;
	pruned_graph.Add_Node(node[current_node].center, 0);
	new_index[current_node] = 1;
	if (node[last_node].degree >= 2){
		++ top;
		stack[0][0] = last_node;
		stack[0][1] = current_node;
	}

	while (true){
		switch (node[current_node].degree){
		case(0):
			throw new Simple_Exception("In Edge_Search: Single Point Error!");
		case(1):
			if (-- top < 0) 
				return;
			current_node = stack[top][0];
			last_node = stack[top][1];
			continue;
		case(2):
			next_node = node[current_node].Find_Another_Adjacent(last_node);
			break;
		default:
		    //若当前点为分叉点，则搜索顺时针方向第一个点为下一点，并且用栈记录该分叉
			double current_angle = //表示current_node结点指向last_node结点的方向角
				atan2(node[last_node].center[0] - node[current_node].center[0], 
				node[last_node].center[1] - node[current_node].center[1])
				+ SKELETONIZE::ANGLE_ERROR;
			if (current_angle > WORM::PI)
				current_angle -= 2 * WORM::PI;
			next_node = Rotate_To_Next(current_node, current_angle);
			stack[top][0] = current_node;
			stack[top][1] = next_node;
			if (top >= SKELETONIZE::STORAGE_MAX)
				throw new Simple_Exception("Edge Search:Stack Overflow!");
			++ top;
		}
		// 若下一点没有被遍历过，则将其加入pruned_graph
		int new_index_1 = new_index[current_node], new_index_2 = new_index[next_node];
		if (new_index_2 == -1){
			pruned_graph.Add_Node(node[next_node].center, new_index_1);
			new_index[next_node] = pruned_graph.node_num - 1;
			last_node = current_node;
			current_node = next_node;
		}
		else{
			if (pruned_graph.node[new_index_1].Adjacent_Index_Locate(new_index_2) == -1)
				pruned_graph.Connect_Node(new_index_1, new_index_2);
			// 搜索栈中首个比next_node更早遍历到的分支点，并且回溯到该分支点，若栈中无这样的点则结束
			while (true){
				if (-- top < 0)
					return;
				if (new_index_2 > new_index[stack[top][0]]){
					current_node = stack[top][0];
					last_node = stack[top][1];
					break;
				}
			}
		}
	}
	if (pruned_graph.node_num < node_num * SKELETONIZE::PRUNE_MINIMUM_PROPORTION)
		throw new Simple_Exception("Skeletonize:Prune Minimum Proportion Too Small!");
}

bool Graph::Calc_End_Direction_Vec(int end_node, double * direction_vec) const{
	if (node[end_node].degree != 1)
		return false;
	int last_node = end_node;
	int current_node = node[end_node].adjacent[0];
	for (int i = 0;i < SKELETONIZE::END_DIRECT_CALC_POINT_NUM;++ i){
		if (node[current_node].degree != 2)
			break;
		//考虑 vec_start_node 未使用的邻接点
		int next_node = node[current_node].Find_Another_Adjacent(last_node);
		last_node = current_node;
		current_node = next_node;
	}
	direction_vec[0] = node[end_node].center[0] - node[current_node].center[0];
	direction_vec[1] = node[end_node].center[1] - node[current_node].center[1];
	return true;
}

int * Graph::Get_End_Node() const{
	end_node_num = 0;
	for (int i = 0;i < node_num;++ i)
		if (node[i].degree == 1){
			if (end_node_num >= SKELETONIZE::STORAGE_MAX)
				throw new Simple_Exception("Too Much Endpoint In The Graph!");
			end_node[end_node_num ++] = i;
		}
	return end_node;
}

void Graph::persistence(void * obj_ptr, string out_file) {
	ofstream file(out_file.c_str(), ios::binary);
	file.write(reinterpret_cast<char *>(obj_ptr), sizeof(Graph));
	file.close();
}

void Graph::anti_persistence(void* obj_ptr, std::string in_file) {
	ifstream file(in_file.c_str(), ios::binary);
	file.read(reinterpret_cast<char *>(obj_ptr), sizeof(Graph));
	file.close();
}