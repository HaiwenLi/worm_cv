#pragma once

#include "CONST_PARA.h"
#include "Graph.h"
#include <opencv2/core/core.hpp>

class Candidate_Points_Detect;

struct Worm_Data{
	Centerline backbone;// 存储计算得到中心线，包含101个等分点的坐标
	Graph graph;

	int pic_num;
	std::string file_tag;
	int worm_area;//线虫面积
	int worm_area_size[2];
	bool adhesion;//线虫是否粘连
	double worm_width[ROOT_SMOOTH::PARTITION_NUM + 1];//线虫身体宽度
	double worm_full_width;
	ROOT_SEARCH::Clockwise_Direct clockwise_head, clockwise_tail, clockwise_whole;
	bool is_first_pic;

	Worm_Data():backbone(ROOT_SMOOTH::PARTITION_NUM + 1), is_first_pic(true),
		adhesion(false), worm_full_width(WORM::INF), worm_area(50000){
			backbone.length = backbone.size;
	};
	void Data_Processing(const Candidate_Points_Detect & detect);
};
