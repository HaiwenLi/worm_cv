﻿#pragma once

#define __OUTPUT_DEBUG_INFO
#define __OUTPUT_STAGE_INFO

namespace WORM {
	const int IMAGE_SIZE = 512;
	const double INF = 1.0E10;
	const double PI = 3.14159265359;
}

namespace BW {
	const int SIDE_WIDTH = 4;
	const double MINIMUM_HOLE_PROPORTION = 0.02;
	extern int BINARY_THRESHOLD;
	const double LAPLACIAN_THRES = -0.3 * 4;	//对距离做laplacian变换后的矩阵中小于此值的点被取为中心线候选点
	const double RAP_THRESHOLD = 0.6;			//宽度大于线虫平均宽度的该倍数时，认为线虫发生了粘连
}

namespace SKELETONIZE {
	const int POINT_NUM_MAX = 1000;
	const int DEGREE_MAX = 5;		//结点最大度数
	const int STORAGE_MAX = 200;
	const int METRICS_MAX = 18;
	const double ANGLE_THRESHOLD_NAN = WORM::PI / 3;   //骨架化时若找不到相邻点则在前方该角度内进行搜索
	const double ALPHA = 2;   //骨架化时若找不到相邻点进行搜索时的参数，alpha越大表示优先选择角度不变的元素
	const double ANGLE_ERROR = 1E-6;
	const int WORM_SPEED = 2; //线虫帧间爬过的长度（以体宽计）
}

namespace ROOT_SMOOTH {
	const int PARTITION_NUM = 100;   //将线虫中心线等分的份数
	const int SMOOTH_DETAIL_LEVEL = 1;   //非负，参数越大平滑细节越好，同时越耗时间
	const int SMOOTH_SCOPE = 5;   //平滑的规模，为0表示不做平滑，参数越大整体越平滑化
	const double ZERO_BOUND = 1.5;
	const int MULTIPLIER = 2 * SMOOTH_DETAIL_LEVEL + 1;
	const int FULL_PARTS = MULTIPLIER * (2 * SMOOTH_SCOPE + 1);
	const int HALF_PARTS = (FULL_PARTS - 1) / 2;
}

const std::string CACHE_DIR = "..\\..\\cache_data\\";

/*
	VIRTUAL_COORDINATE是对于目前系统而言定义的虚拟坐标系（满足右手法则），该坐标系规定：
	1、从下至上，垂直培养皿的方向为Z轴正方向
	2、在培养皿平面看，从左至右为X轴正方向（左右前后是站在目前系统前方而言的）
	3、在培养皿平面看，从前至后为Y轴正方向

	注：虚拟坐标系中的变化矩阵均是对于正方向而言的
	*/
namespace VIRTUAL_COORDINATE {
	const int X_POSITIVE = 1;//虚拟坐标系X正轴
	const int Y_POSITIVE = 1;//虚拟坐标系Y正轴
	const int Z_POSITIVE = 1;//虚拟坐标系Z正轴

	const double XY_TO_IMAGE_MATRIX[2][2] = { 0, 31, 31, 0 };//电机XY轴对应到图像空间的变化矩阵
}

class Simple_Exception {
private:
	std::string message;
public:
	explicit Simple_Exception(std::string message) :message(message) {}
	void Add_message(std::string new_message) {
		message += new_message;
	}
	std::string Get_message() {
		return message;
	}
};
