#pragma once

#include "CONST_PARA.h"

struct Centerline;
class Graph;

struct Root_In_Graph{
	int * index, length, size;//index存储节点编号，length在调用Add_Node后会变化
	ROOT_SEARCH::Root_Circle circle;
	int adhesion_index; // 仅在circle不为NONE时有意义，记录成环的连接节点坐标

	Root_In_Graph(int root_max_length):length(0), size(root_max_length), circle(ROOT_SEARCH::NONE), adhesion_index(-1){
		index = new int[size];
	}
	Root_In_Graph(const Root_In_Graph & root);
	~Root_In_Graph(){
		delete[] index;
	}
	Root_In_Graph & operator=(const Root_In_Graph & root);
	int & operator[](int n){
		if (n < 0 || n >= length)
			throw new Simple_Exception("Root_In_Graph:Invalid Index Error!");
		return index[n];
	}
	int operator[](int n) const{
		if (n < 0 || n >= length)
			throw new Simple_Exception("Root_In_Graph:Invalid Index Error!");
		return index[n];
	}
	// 需要保证输入node在graph中表示合法的结点编号
	void Add_Node(int node){
		if (length >= size || index == nullptr)
			throw new Simple_Exception("Root_Length Error!");
		index[length ++] = node;
	}
};

class Root_Search{
private:
	const Graph & graph;
	Centerline *centerline_candidates[SKELETONIZE::STORAGE_MAX];
	int root_num;//搜索的路径数
	int max_length;//最大路径长度
	bool longest_root_not_circle;//最大路径是否存在圈
	
	// 获取路径root_in_graph中节点的中心坐标，并存入centerline中
	void Obtain_Coodinate(const Root_In_Graph & root_in_graph, Centerline & centerline);
	// 删除下标为n的路径
	void Delete_Root(int n);
	// 删除短路径
	void Short_Root_Filter(bool circle_mode);
	// 判断线虫头尾
	void Match_Correct_Head(const Centerline & last_backbone, ROOT_SEARCH::Clockwise_Direct cloockwise_full);
	// 将new_root表示的路径加入到中心线序列
	void Root_Add(const Root_In_Graph & new_root, double worm_full_width);
	// 从中心线序列（centerline_candidates）中选择合适的路径作为线虫的中心线
	void Root_Select(ROOT_SEARCH::Clockwise_Direct cloockwise_head, ROOT_SEARCH::Clockwise_Direct cloockwise_tail);
public:
	Root_Search(Graph & graph);
	void Search_Backbone(Centerline & last_backbone, ROOT_SEARCH::Clockwise_Direct cloockwise_full, 
		ROOT_SEARCH::Clockwise_Direct cloockwise_head, ROOT_SEARCH::Clockwise_Direct cloockwise_tail, double worm_full_width, bool first_pic = true);
};
