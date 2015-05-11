#include "stdafx.h"

using namespace std;
using namespace cv;

Worm_Data worm_data;

Centerline *Search_Backbone(Mat image, string pic_num_str = "New Pic"){

	string cache_dir = "..\\..\\cache_data\\";
	static Candidate_Points candidate_center_points;
	static Graph skeleton_graph;
	static Graph pruned_graph;
	static Candidate_Points_Detect candidate_points_detect;
	static Skeletonize skeletonize_method;

	skeleton_graph.Reset();
	pruned_graph.Reset();
	candidate_center_points.Reset();

	candidate_points_detect.Detect_Points(image, candidate_center_points, worm_data.worm_full_width, worm_data.worm_area);
	Time_Calc();
#ifndef __SKIP_DEBUG_INFO
	cout<<"Candidates Detect Complete!"<<endl;
	candidate_points_detect.Save2File(cache_dir + "dist_mat\\", cache_dir + "lap_mat\\", pic_num_str);
	candidate_center_points.Save2File(cache_dir+"candidate_points\\"+pic_num_str);
#endif
	skeletonize_method.Convert_To_Graph(& candidate_center_points, & skeleton_graph);
#ifndef __SKIP_DEBUG_INFO
	cout<<"Skeletonize Complete!"<<endl;
	skeleton_graph.Save2File(cache_dir+"graph_unpruned\\"+pic_num_str);
#endif 
	skeleton_graph.Edge_Search(pruned_graph);
#ifndef __SKIP_DEBUG_INFO
	cout<<"Graph Prune Complete!"<<endl;
	pruned_graph.Save2File(cache_dir+"graph_pruned\\"+pic_num_str);
#endif
	Root_Search(pruned_graph).Search_Backbone();
#ifndef __SKIP_DEBUG_INFO
	cout<<"Backbone Search Complete!"<<endl;
	worm_data.backbone.Save2File(cache_dir + "backbone_unsmoothed\\"+pic_num_str);
#endif
	Root_Smooth().Interpolate_And_Equal_Divide(worm_data.backbone, ROOT_SMOOTH::PARTITION_NUM);
#ifndef __SKIP_DEBUG_INFO
	cout<<"Smooth Complete!"<<endl;
	worm_data.backbone.Save2File(cache_dir + "backbone_smoothed\\"+pic_num_str);
#endif
	worm_data.Data_Processing(candidate_points_detect);
	cout << worm_data.worm_area;
	return &worm_data.backbone;
}
