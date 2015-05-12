#include "stdafx.h"

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
	clockwise_head = backbone.Clockwise_Direct_Calc(0, PARTITION_NUM / 2);
	clockwise_tail = backbone.Clockwise_Direct_Calc(PARTITION_NUM / 2, PARTITION_NUM - 1);
	clockwise_whole = backbone.Clockwise_Direct_Calc(0, PARTITION_NUM - 1);
	first_pic = false;
	delete[] worm_width;
}

void Search_Backbone::Data_Save() {
	static auto candidate_ptr = reinterpret_cast<int *>(&candidate_center_points);
	static auto skeleton_graph_ptr = reinterpret_cast<int *>(&skeleton_graph);
	static auto pruned_graph_ptr = reinterpret_cast<int *>(&pruned_graph);
	static auto backbone_ptr = reinterpret_cast<int *>(&backbone);

	static int *obj_ptrs[FINISH] = {nullptr, candidate_ptr, skeleton_graph_ptr, pruned_graph_ptr, backbone_ptr, backbone_ptr};
	static void (* func_ptrs[FINISH])(int *obj_ptr, string file_dir) = {nullptr, Candidate_Points::consistense_serialize, 
		Graph::Save2File, Graph::Save2File, Centerline::Save2File, Centerline::Save2File };
	static string cache_dir_str[FINISH] = { "", "candidate_points\\", "graph_unpruned\\", "graph_pruned\\", "backbone_unsmoothed\\", "backbone_smoothed\\" };

	auto pic_num_str = num2str(pic_num);
#ifdef __OUTPUT_DEBUG_INFO
	for (int i = CANDIDATE; i <= current_stage; ++i){
		func_ptrs[i](obj_ptrs[i], cache_dir + cache_dir_str[i] + pic_num_str);
	}
#endif
}

void Search_Backbone::Next_Stage(){
	static string stage_words[FINISH] = { "Candidates Detect Complete!", "Skeletonize Complete!", "Graph Prune Complete!", 
		"Backbone Search Complete!", "Smooth Complete!", "All Finished!\n"};
#ifdef __OUTPUT_STAGE_INFO
	cout << stage_words[current_stage] << endl;
#endif
	current_stage = Stage((current_stage+1) % FINISH);
}


const Centerline *Search_Backbone::Search(const Mat & image){
	string pic_num_str = num2str(pic_num);
	skeleton_graph.Reset();
	pruned_graph.Reset();
	candidate_center_points.Reset();
	candidate_points_detect.Detect_Points(image, candidate_center_points, worm_full_width, worm_area);
	Next_Stage();
	skeletonize.Convert_To_Graph(& candidate_center_points, & skeleton_graph, pic_num_str);
	Next_Stage(); 
	skeleton_graph.Edge_Search(pruned_graph);
	Next_Stage();
	Root_Search(pruned_graph).Search_Backbone(backbone, clockwise_whole, clockwise_head, clockwise_tail, worm_full_width, first_pic);
	Next_Stage();
	root_smooth.Interpolate_And_Equal_Divide(backbone, ROOT_SMOOTH::PARTITION_NUM);
	Next_Stage();
	Data_Processing();
	Data_Save();
	Next_Stage();
	++ pic_num;
	return &backbone;
}
