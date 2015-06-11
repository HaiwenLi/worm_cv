#pragma once

#include "Backbone.h"
#include "Candidate_Points.h"
#include "Graph_Builder.h"
#include "Graph_Prune.h"
#include "Root_Smooth.h"
#include "Graph.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

class Search_Backbone {
private:
	//method
	Candidate_Points_Detect candidate_points_detect;
	Graph_Builder skeletonize;
	Graph_Prune graph_prune;
	Root_Smooth root_smooth;
	//data
	Candidate_Points candidate_center_points;
	Graph skeleton_graph;
	Backbone backbone;
	
	std::string pic_num_str;
	bool first_pic;
	double worm_full_width;
	double worm_area;

	void Data_Processing();
	void Next_Stage();
	static void persistence(void * obj_ptr, std::string out_file);
public:
	explicit Search_Backbone() :backbone(ROOT_SMOOTH::PARTITION_NUM + 1), first_pic(true), worm_full_width(WORM::INF), worm_area(WORM::INF){}
	void Set_Width(double worm_full_width) {
		this->worm_full_width = worm_full_width;
	}
	void Set_Area(double worm_area){
		this->worm_area = worm_area;
	}
	const Backbone * Search(const cv::Mat & image, std::string pic_num_str = "temp");
};