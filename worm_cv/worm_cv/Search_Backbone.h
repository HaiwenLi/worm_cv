#pragma once

#include "Candidate_Points.h"
#include "Skeletonize.h"
#include "Centerline.h"
#include "Root_Search.h"
#include "Root_Smooth.h"
#include "Graph.h"
#include "Graph_Prune.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

class Search_Backbone {
private:
	//method
	Candidate_Points_Detect candidate_points_detect;
	Skeletonize skeletonize;
	Root_Smooth root_smooth;
	//data
	Candidate_Points candidate_center_points;
	Graph skeleton_graph;
	Graph pruned_graph;
	Graph_Prune graph_prune;
	Centerline backbone;

	typedef enum { INIT, CANDIDATE, GRAPH, PRUNE, BACKBONE, SMOOTH, FINISH } Stage;
	Stage current_stage;

	int pic_num;
	bool first_pic;
	double worm_full_width;
	double worm_area;
	ROOT_SEARCH::Clockwise_Direct clockwise_head, clockwise_tail, clockwise_whole;

	void Data_Processing();
	void Next_Stage();
	static void persistence(void * obj_ptr, std::string out_file);
public:
	explicit Search_Backbone(int start_num = 0) :backbone(ROOT_SMOOTH::PARTITION_NUM + 1), pic_num(0), first_pic(true), worm_full_width(WORM::INF), worm_area(WORM::INF){
		this->pic_num = start_num;
	}
	void Set_Width(double worm_full_width) {
		this->worm_full_width = worm_full_width;
	}
	void Set_Area(double worm_area){
		this->worm_area = worm_area;
	}
	const Centerline * Search(const cv::Mat & image);
};