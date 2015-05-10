#pragma once

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <cmath>
#include <string>
#include "Candidate_Points.h"
#include "Skeletonize.h"
#include "Centerline.h"
#include "Root_Search.h"
#include "Root_Smooth.h"
#include "Worm_Data.h"
#include "Graph.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

inline bool Is_8_Neighbour(const int * point_1, const int * point_2){
	return point_1[0] - point_2[0] < 2 && point_2[0] - point_1[0] < 2 
		&& point_1[1] - point_2[1] < 2 && point_2[1] - point_1[1] < 2;
}

template<typename T1, typename T2>
double Point_Dist_Square(const T1 * p1, const T2 * p2){
	return (p1[0] - p2[0])*(p1[0] - p2[0])+(p1[1] - p2[1])*(p1[1] - p2[1]);
}

template<typename T1, typename T2>
double Included_Angle_Tan(const T1 * vec_1, const T2 * vec_2){
	// 做旋转变换将vec_1旋转到y轴正方向后vec_2的方向
	double angle[2] = {vec_2[0]*vec_1[1] - vec_2[1]*vec_1[0], vec_1[0]*vec_2[0] + vec_1[1]*vec_2[1]};
	if (angle[0] < 0) 
		angle[0] = - angle[0];
	return angle[1] <= 1E-5 ? -1 : angle[0]/angle[1];
}

// 有序数组二分查找
template<class T>
int Binary_Chop(const T * ordered_array, T element_to_locate, int left, int right){
	if (element_to_locate < ordered_array[left] || element_to_locate > ordered_array[right])
		return -1;
	if (element_to_locate == ordered_array[right])
		return right;
	while (right - left > 1){
		int mid = (left + right) / 2;
		if (ordered_array[mid] < element_to_locate)
			left = mid;
		else if (ordered_array[mid] > element_to_locate)
			right = mid;
		else return mid;
	}
	return left;
}

// 将整型变量num转换为字符串型返回
template <class T>
inline std::string num2str(T num){	
	std::stringstream stream;
	std::string s;
	stream << num;
	stream >> s;
	return s;
}

class Select_Minimum{
private:
	double min_Val;
	int min_Index;
public:
	Select_Minimum(double initial_val, int initial_index):
		min_Val(initial_val), min_Index(initial_index){};
	void Renew(double new_val, int new_index){
		if (new_val < min_Val){
			min_Val = new_val;
			min_Index = new_index;
		}
	}
	int Get_Min_Index() const{
		return min_Index;
	}
};

template<typename T>
void Save_Mat_To_File(const cv::Mat & mat, std::string file_dir){
	ofstream file(file_dir.c_str(), ios::binary);
	for (int i = 0;i < mat.rows;++ i){
		const T * ptr = mat.ptr<T>(i);
		file.write((char *)ptr, mat.cols * sizeof(T));
	}
	file.close();
}

extern Worm_Data worm_data;

const int PARTS = 12;
extern clock_t _t[PARTS], __t;
extern long t[PARTS], __t_;
extern int cparts;
inline void Time_Calc(){
	_t[cparts + 1] = clock();
	t[cparts] += _t[cparts+1] - _t[cparts];
	++ cparts;
}
