#include "stdafx.h"

using namespace std;
using namespace cv;

void Contour_Range_Get(vector<Point> worm_contour, int * Range){
	int top = worm_contour[0].x,
		bottom = worm_contour[0].x,
		left = worm_contour[0].y,
		right = worm_contour[0].y;
	for (unsigned i = 1; i < worm_contour.size(); ++i){
		if (worm_contour[i].x < top)	top = worm_contour[i].x;
		if (worm_contour[i].x > bottom) bottom = worm_contour[i].x;
		if (worm_contour[i].y < left)	left = worm_contour[i].y;
		if (worm_contour[i].y > right)	right = worm_contour[i].y;
	}
	Range[0] = max(top - BW::SIDE_WIDTH, 1);
	Range[1] = min(bottom + BW::SIDE_WIDTH, WORM::IMAGE_SIZE);
	Range[2] = max(left - BW::SIDE_WIDTH, 1);
	Range[3] = min(right + BW::SIDE_WIDTH, WORM::IMAGE_SIZE);
}

void Denoise_And_Worm_Locate(Mat & binary_image){
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	findContours(binary_image, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);//调用CV函数获取轮廓线

	Select_Minimum select_contours_by_area(WORM::INF, -1);
	for (unsigned i = 0; i < contours.size(); ++i){
		double area_diff = abs(contourArea(contours[i]) - worm_data.worm_area);
		select_contours_by_area.Renew(area_diff, i);
	}
	int contour_select = select_contours_by_area.Get_Min_Index();
	if (contour_select == -1)
		throw new Simple_Exception("Can't get connected components of the worm");
	Contour_Range_Get(contours[contour_select], worm_data.Worm_XY_Range);
	double minimum_whole_area = contourArea(contours[contour_select]) * BW::MINIMUM_WHOLE_PROPORTION;
	// Retain a contour whose area is closest to area
	for (int i = 0; i >= 0; i = hierarchy[i][0])
		drawContours(binary_image, contours, i, 255 * (i == contour_select), CV_FILLED, 8, hierarchy);

	//这个binary_image表示裁剪之后的图像数组，采用指针的方式返回，也可以使用引用类型的返回
	binary_image = Mat(binary_image,
		Range(worm_data.Worm_XY_Range[2], worm_data.Worm_XY_Range[3]),
		Range(worm_data.Worm_XY_Range[0], worm_data.Worm_XY_Range[1]));
	binary_image = ~binary_image;

	//----------------------填充线虫区域内部的空白杂点-----------------------
	contours.clear();//清空所有数据
	hierarchy.clear();
	findContours(binary_image, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
	//填充线虫区域内部的空白杂点
	for (int i = 0; i >= 0; i = hierarchy[i][0]){
		bool retain_whole = contourArea(contours[i]) > minimum_whole_area;
		drawContours(binary_image, contours, i, 255 * retain_whole, CV_FILLED, 8, hierarchy);
	}
	binary_image = ~binary_image;

	//更新图片的大小
	worm_data.worm_area_size[0] = binary_image.rows;
	worm_data.worm_area_size[1] = binary_image.cols;
	//计算线虫的面积，这里可以使用GPU加速
	worm_data.worm_area = 0;//初始化
	for (int i = 0; i < binary_image.rows; ++i){
		uchar * Mi = binary_image.ptr<uchar>(i);
		for (int j = 0; j < binary_image.cols; ++j)
			worm_data.worm_area += (Mi[j] != 0);
		Mi[0] = 0;
		Mi[binary_image.cols-1] = 0;
	}
	uchar * first_line = binary_image.ptr<uchar>(0);
	uchar * end_line = binary_image.ptr<uchar>(binary_image.rows-1);
	for (int i = 1;i < binary_image.cols-1;++ i){
		first_line[i] = 0;
		end_line[i] = 0;
	}
}
