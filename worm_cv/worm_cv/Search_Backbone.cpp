﻿#include "stdafx.h"

using namespace std;
using namespace cv;

using namespace ROOT_SMOOTH;

void Search_Backbone::Data_Processing() {
	double * worm_width = new double[backbone.length];
	for (int i = 0; i <= PARTITION_NUM; ++i) {
		worm_width[i] = candidate_points_detect.Get_Dist(backbone.cood[i][0], backbone.cood[i][1]);
		}
	worm_area = candidate_points_detect.Get_Area();
	sort(worm_width, worm_width + PARTITION_NUM);
	worm_full_width = 2 * worm_width[int(PARTITION_NUM * 0.8)];
	first_pic = false;
	delete[] worm_width;
}

void Search_Backbone::persistence(void* obj_ptr, std::string out_file){
	auto typed_ptr = reinterpret_cast<Search_Backbone *>(obj_ptr);
	ofstream file(out_file.c_str(), ios::binary);
	file.write(reinterpret_cast<char *>(&typed_ptr->worm_full_width), sizeof(double));
	file.write(reinterpret_cast<char *>(&typed_ptr->worm_area), sizeof(double));
	file.close();
}

void Search_Backbone::Next_Stage() {
	typedef enum { INIT, CANDIDATE, GRAPH, GET_BACKBONE, SMOOTH, FINISH } Stage;
	static auto current_stage = INIT;
	static string stage_words[FINISH] = { "Candidates Detect Complete!", "Skeletonize Complete!", 
		"Graph Prune Complete, Backbone Get!", "Smooth Complete!", "All Finished!\n" };
	static void *persist_obj_ptrs[FINISH] = { &candidate_center_points, &skeleton_graph, &backbone, &backbone, this};
	static void(*persist_fun_ptrs[FINISH])(void *obj_ptr, string file_dir) = { Candidate_Points::persistence,
		Graph::persistence, Backbone::persistence, Backbone::persistence, persistence};
	static string cache_dir_strs[FINISH] = {"candidate_points\\", "graph_unpruned\\", "backbone_unsmoothed\\", "backbone_smoothed\\", "worm_data\\" };
#ifdef __OUTPUT_STAGE_INFO
	cout << stage_words[current_stage] << endl;
#endif
#ifdef __OUTPUT_DEBUG_INFO
	persist_fun_ptrs[current_stage](persist_obj_ptrs[current_stage], CACHE_DIR + cache_dir_strs[current_stage] + pic_num_str);
#endif
	current_stage = Stage((current_stage + 1) % FINISH);
}

const Backbone *Search_Backbone::Search(const Mat & image, string pic_num_str){
	skeleton_graph.Reset();
	candidate_center_points.Reset();
	this->pic_num_str = pic_num_str;
	cout << "Pic:" << pic_num_str << endl;
	candidate_points_detect.Detect_Points(image, candidate_center_points, worm_full_width, worm_area);
	candidate_points_detect.Save2File("..\\..\\cache_data\\dist_mat\\","..\\..\\cache_data\\lap_mat\\",pic_num_str);
	Next_Stage();
	skeletonize.Convert_To_Graph(& candidate_center_points, & skeleton_graph, pic_num_str);
	Next_Stage();
	graph_prune.Prune(&skeleton_graph, backbone, worm_full_width, first_pic);
	Next_Stage();
	root_smooth.Interpolate_And_Equal_Divide(backbone, ROOT_SMOOTH::PARTITION_NUM);
	Next_Stage();
	Data_Processing();
	Next_Stage();
	return &backbone;
}
