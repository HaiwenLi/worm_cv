#pragma once

#include "CONST_PARA.h"

struct Centerline{
	int length;
	int size;
	double (* cood)[2];
	ROOT_SEARCH::Root_Circle circle; // 记录路径首尾的粘连情况
	int adhesion_index; // 仅在circle不为NONE时有意义，记录粘连处的连接节点编号

	explicit Centerline(int root_max_length):length(0), size(root_max_length), circle(ROOT_SEARCH::NONE), adhesion_index(-1){
		cood = new double[size][2];
	}
	Centerline(const Centerline & root);
	Centerline & operator=(const Centerline & root);
	~Centerline(){
		delete[] cood;
	}
	bool Have_Same_Endpoints(const Centerline & root_to_match) const{
		if (circle != root_to_match.circle)
			return false;
		// 检查是否首尾相同
		if (cood[0][0] == root_to_match.cood[0][0] &&
			cood[0][1] == root_to_match.cood[0][1] &&
			cood[length - 1][0] == root_to_match.cood[root_to_match.length - 1][0] &&
			cood[length - 1][1] == root_to_match.cood[root_to_match.length - 1][1])
			return true;
		// 检查是否首尾反向
		if (cood[length - 1][0] == root_to_match.cood[0][0] &&
			cood[length - 1][1] == root_to_match.cood[0][1] &&
			cood[0][0] == root_to_match.cood[root_to_match.length - 1][0] &&
			cood[0][1] == root_to_match.cood[root_to_match.length - 1][1])
			return true;
		return false;
	}
	void Reverse();//中心线反转
	void Tail_Cut(double full_width);//去除中心线尾部点
	ROOT_SEARCH::Clockwise_Direct Clockwise_Direct_Calc(int start_index, int tail_index);//计算时针方向
	void Save2File(std::string file_name);
};

