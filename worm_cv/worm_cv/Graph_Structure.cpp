#include "stdafx.h"

Graph_Structure::Graph_Structure(int real_node_num, int structure_node_max) :node_num(0) {
	nodes = new Graph_Structure_Node[structure_node_max];
	node_hash = new int[real_node_num];
	for (auto i = 0; i < real_node_num; ++i)
		node_hash[i] = -1;
	for (auto i = 0; i < structure_node_max; ++i)
		nodes[i].degree = 0;
}

Graph_Structure::~Graph_Structure() {
	if (nodes != nullptr)
		delete[] nodes;
	if (node_hash != nullptr)
		delete[] node_hash;
}

void Graph_Structure::Check_Structure() {
	for (auto node_index = 0; node_index < node_num; ++node_index) {
		auto & structure_node_0 = nodes[node_index];
		// 度为2的点可删除
		if (structure_node_0.degree != 2)
			return;
		// 连接当前点项链的两条边
		Graph_Structure_Node * adjacents[2];
		for (auto i = 0; i < 2; ++i) {
			adjacents[i] = nodes + node_hash[structure_node_0.edge_end[i]];
			for (auto j = 0; j < adjacents[i]->degree; ++j) {
				if (adjacents[i]->edge_midway_1[j] == structure_node_0.edge_midway_2[i]) {
					adjacents[i]->edge_midway_2[j] = structure_node_0.edge_midway_2[1 - i];
					adjacents[i]->edge_len[j] = structure_node_0.edge_len[0] + structure_node_0.edge_len[1];
					adjacents[i]->edge_end[j] = structure_node_0.edge_end[1 - i];
				}
			}
		}
		structure_node_0.degree = 0;
		node_hash[structure_node_0.real_index] = -1;
	}
}

void Graph_Structure::Add_Edge_Oneway(int edge_start, int edge_end, int edge_len, int midway1, int midway2) {
	if (node_hash[edge_start] < 0)
		node_hash[edge_start] = node_num++;
	auto & start_node = nodes[node_hash[edge_start]];
	start_node.edge_midway_1[start_node.degree] = midway1;
	start_node.edge_midway_2[start_node.degree] = midway2;
	start_node.edge_len[start_node.degree] = edge_len;
	start_node.edge_end[start_node.degree] = edge_end;
}

void Graph_Structure::Delete_Edge_Oneway(int edge_start, int midway1) {
	if (node_hash[edge_start] < 0) {
		throw new Simple_Exception("Graph_Structure : Try To Delete Non-Structure-Node");
	}
	auto & deleted_node = nodes[node_hash[edge_start]];
	--deleted_node.degree;
	for (auto i = 0; i < deleted_node.degree; ++i) {
		if (deleted_node.edge_midway_1[i] == midway1) {
			deleted_node.edge_midway_1[i] = deleted_node.edge_midway_1[deleted_node.degree];
			deleted_node.edge_midway_2[i] = deleted_node.edge_midway_2[deleted_node.degree];
			deleted_node.edge_len[i] = deleted_node.edge_len[deleted_node.degree];
			deleted_node.edge_end[i] = deleted_node.edge_end[deleted_node.degree];
		}
	}
}

void Graph_Structure::Add_Edge(int edge_start, int edge_end, int edge_len, int midway1, int midway2) {
	Add_Edge_Oneway(edge_start, edge_end, edge_len, midway1, midway2);
	Add_Edge_Oneway(edge_end, edge_start, edge_len, midway2, midway1);
}

void Graph_Structure::Delete_Edge(int edge_start, int edge_end, int midway1, int midway2) {
	Delete_Edge_Oneway(edge_start, midway1);
	Delete_Edge_Oneway(edge_end, midway2);
}

bool Graph_Structure::In_Edge_Check(int& last_node, int& current_node) {
	auto start_hash = node_hash[last_node];
	if (start_hash < 0)
		return false;
	auto & start_node = nodes[start_hash];
	for (auto i = 0; i < start_node.degree; ++i) {
		if (start_node.edge_midway_1[i] == current_node) {
			last_node = start_node.edge_midway_2[i];
			current_node = start_node.edge_end[i];
			return true;
		}
	}
	return false;
}