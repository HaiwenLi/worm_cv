#include "stdafx.h"

using namespace std;
using namespace cv;

//===================================================函数列表==================================================
void Image_Get(Mat & binary_image, int img_index);
void Denoise_And_Worm_Locate(Mat & binary_image);

Worm_Data worm_data;

clock_t _t[PARTS], __t, __t_ = 0;
long t[PARTS] = {};
int cparts;

Centerline *Search_Backbone(Mat image, string pic_num_str = "New Pic"){

	string cache_dir = "..\\..\\cache_data\\";
	static Candidate_Points candidate_center_points;
	static Graph skeleton_graph;
	static Candidate_Points_Detect candidate_points_detect;
	static Skeletonize skeletonize_method;

	skeleton_graph.Reset();
	candidate_center_points.Reset();

	candidate_points_detect.Detect_Points(image, candidate_center_points, worm_data.worm_full_width);
	Time_Calc();
#ifndef __SKIP_DEBUG_INFO
	cout<<"Candidates Detect Complete!"<<endl;
	candidate_points_detect.Save2File(cache_dir, pic_num_str);
	candidate_center_points.Save2File(cache_dir+"candidate_points\\"+pic_num_str);
#endif
	skeletonize_method.Convert_To_Graph(& candidate_center_points, & skeleton_graph);
#ifndef __SKIP_DEBUG_INFO
	cout<<"Skeletonize Complete!"<<endl;
	skeleton_graph.Save2File(cache_dir+"graph_unpruned\\"+pic_num_str);
#endif 
	skeleton_graph.Edge_Search();
#ifndef __SKIP_DEBUG_INFO
	cout<<"Graph Prune Complete!"<<endl;
	worm_data.graph.Save2File(cache_dir+"graph_pruned\\"+pic_num_str);
#endif
	Root_Search().Search_Backbone();
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
	return &worm_data.backbone;
}

int main(){
	Mat image;
	clock_t _start = clock();
	int PIC_NUM = 500;
	for (worm_data.pic_num = 0;worm_data.pic_num < PIC_NUM;++ worm_data.pic_num){
		cparts = 0;
		_t[cparts] = clock();
		Image_Get(image, worm_data.pic_num);
		Search_Backbone(image, num2str(worm_data.pic_num+1001));
		t[cparts] += clock() - _t[cparts];
	}
	long tall = clock()-_start;
	cout<<"total time:\t"<<tall<<"us"<<endl;
	cout<<"exclude read:\t"<<tall - t[0]<<"us"<<endl;
	cout<<"time per pic:\t"<<(clock()-_start)/double(PIC_NUM)<<endl;
	cout<<"exclude read:\t"<<(tall - t[0])/(double(PIC_NUM))<<endl;
	for (int i = 0; i <= cparts; i++){
		cout<<"Part "<<i+1<<":  "<<t[i]<<"ms\t\tPer pic:"<<t[i]/double(PIC_NUM)<<endl;
		cout<<"Percentage:"<<setprecision(3)<< 100.0*t[i]/tall<<"\n";
	}
	//cout<<__t_<<endl;
	system("pause");
	return 0;
}

void Image_Get(Mat & image, int img_index){
	//string image_filename = "/home/qiaohan/beng_fig.tiff";
	string image_filename = "..\\..\\worm_pic\\" + num2str(img_index + 1001) + ".tiff";
	image = imread(image_filename, 0);
	if (!image.data)
		throw new Simple_Exception("Could not open or find the image");
	if (image.rows != WORM::IMAGE_SIZE || image.cols != WORM::IMAGE_SIZE)
		throw new Simple_Exception("Image Size Not Fit!");
}
