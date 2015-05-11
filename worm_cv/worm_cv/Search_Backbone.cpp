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
	++pic_num;
	delete[] worm_width;
}

const Centerline* Search_Backbone::Search(const cv::Mat& image){
	auto pic_num_str = num2str(pic_num);

	candidate_center_points.Reset();
	skeleton_graph.Reset();
	pruned_graph.Reset();

	candidate_points_detect.Detect_Points(image, candidate_center_points, worm_full_width, worm_area);
#ifndef __SKIP_DEBUG_INFO
	cout << "Candidates Detect Complete!" << endl;
	candidate_points_detect.Save2File(cache_dir + "dist_mat\\", cache_dir + "lap_mat\\", pic_num_str);
	candidate_center_points.Save2File(cache_dir + "candidate_points\\" + pic_num_str);
#endif
	skeletonize.Convert_To_Graph(&candidate_center_points, &skeleton_graph, pic_num_str);
#ifndef __SKIP_DEBUG_INFO
	cout << "Skeletonize Complete!" << endl;
	skeleton_graph.Save2File(cache_dir + "graph_unpruned\\" + pic_num_str);
#endif 
	skeleton_graph.Edge_Search(pruned_graph);
#ifndef __SKIP_DEBUG_INFO
	cout << "Graph Prune Complete!" << endl;
	pruned_graph.Save2File(cache_dir + "graph_pruned\\" + pic_num_str);
#endif
	Root_Search(pruned_graph).Search_Backbone(backbone, clockwise_whole, clockwise_head, clockwise_tail, worm_full_width, first_pic);
#ifndef __SKIP_DEBUG_INFO
	cout << "Backbone Search Complete!" << endl;
	backbone.Save2File(cache_dir + "backbone_unsmoothed\\" + pic_num_str);
#endif
	Root_Smooth().Interpolate_And_Equal_Divide(backbone, ROOT_SMOOTH::PARTITION_NUM);
#ifndef __SKIP_DEBUG_INFO
	cout << "Smooth Complete!" << endl;
	backbone.Save2File(cache_dir + "backbone_smoothed\\" + pic_num_str);
#endif
	Data_Processing();
	return &backbone;
	}
