#include "stdafx.h"

using namespace std;
using namespace cv;
using namespace WORM;

const double Candidate_Points_Detect::HALF_THRES = BW::LAPLACIAN_THRES / 2;

//private:
void Candidate_Points_Detect::Distance_Retrace(double width_max){
	for (int i = 0; i < distance_matrix.rows; ++i){
		float * dist_row = distance_matrix.ptr<float>(i);
		for (int j = 0; j < distance_matrix.cols; ++j)
			if (dist_row[j] > BW::RAP_THRESHOLD * width_max)
				dist_row[j] = fmod(width_max - static_cast<double>(dist_row[j]), width_max) * BW::RAP_SLOPE;
	}
}

void Candidate_Points_Detect::Calc_LapMat_Of_Inner_Part() const{
	const float * dist_row[IMAGE_SIZE];
	for (int i = 0; i < distance_matrix.rows; ++i)
		dist_row[i] = distance_matrix.ptr<float>(i);

	for (int i = 1;i < distance_matrix.rows - 1;++ i)
		for (int j = 1;j < distance_matrix.cols - 1;++ j)
			if (dist_row[i][j] > 2){
				laplacian_matrix[i][j] = dist_row[i-1][j] + dist_row[i+1][j] + dist_row[i][j-1] + dist_row[i][j+1] - 4*dist_row[i][j];
				if (laplacian_matrix[i][j] > 0)
					laplacian_matrix[i][j] = 0;
			}
			else
				laplacian_matrix[i][j] = 0;
	for (int i = 0;i < distance_matrix.rows;++ i){
		laplacian_matrix[i][0] = 0;
		laplacian_matrix[i][distance_matrix.cols-1] = 0;
	}
	for (int i = 1;i < distance_matrix.cols - 1;++ i){
		laplacian_matrix[0][i] = 0;
		laplacian_matrix[distance_matrix.rows-1][i] = 0;
	}
}

bool Candidate_Points_Detect::Lap_Value_Small_Enough(int i, int j) const{
	if (laplacian_matrix[i][j] < BW::LAPLACIAN_THRES)
		return true;
	if (laplacian_matrix[i][j] >= HALF_THRES)
		return false;
	return 
		(laplacian_matrix[i-1][j-1] < HALF_THRES ? 1 : 0) +
		(laplacian_matrix[i-1][ j ] < HALF_THRES ? 1 : 0) +
		(laplacian_matrix[i-1][j+1] < HALF_THRES ? 1 : 0) +
		(laplacian_matrix[ i ][j-1] < HALF_THRES ? 1 : 0) +
		(laplacian_matrix[ i ][j+1] < HALF_THRES ? 1 : 0) +
		(laplacian_matrix[i+1][j-1] < HALF_THRES ? 1 : 0) +
		(laplacian_matrix[i+1][ j ] < HALF_THRES ? 1 : 0) +
		(laplacian_matrix[i+1][j+1] < HALF_THRES ? 1 : 0) >= 4;
}

void Candidate_Points_Detect::Catch_Candidate_By_LapMat(Candidate_Points &candidate_Points){
	for (int i = 0; i < Worm_XY_Range[2]-1; i++)
		candidate_Points.Add_Line();
	for (int i = 1; i < distance_matrix.rows - 1; ++ i){
		candidate_Points.Add_Line();
		for (int j = 1; j < distance_matrix.cols - 1;++ j)
			if (Lap_Value_Small_Enough(i,j))
				candidate_Points.Add_Point_To_Line(j+Worm_XY_Range[0]);
	}
	candidate_Points.Add_Line();
	candidate_Points.Add_Line();
}

void Candidate_Points_Detect::Contour_Range_Get(vector<Point> worm_contour, int * Range){
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

void Candidate_Points_Detect::Denoise_And_Worm_Locate(double area){
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	findContours(binary_image, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);//调用CV函数获取轮廓线

	Select_Minimum select_contours_by_area(WORM::INF, -1);
	for (unsigned i = 0; i < contours.size(); ++i)
		select_contours_by_area.Renew(abs(area-contourArea(contours[i])), i);
	int contour_select = select_contours_by_area.Get_Min_Index();
	this -> area = contourArea(contours[contour_select]);
	if (contour_select == -1)
		throw new Simple_Exception("Can't get connected components of the worm");

	Contour_Range_Get(contours[contour_select], Worm_XY_Range);
	// Retain a contour whose area is closest to area
	for (int i = 0; i >= 0; i = hierarchy[i][0])
		drawContours(binary_image, contours, i, 255 * (i == contour_select), CV_FILLED, 8, hierarchy);

	//这个binary_image表示裁剪之后的图像数组，采用指针的方式返回，也可以使用引用类型的返回
	binary_image = Mat(binary_image,
		Range(Worm_XY_Range[2], Worm_XY_Range[3]),
		Range(Worm_XY_Range[0], Worm_XY_Range[1]));
	binary_image = ~binary_image;

	//----------------------填充线虫区域内部的空白杂点-----------------------
	contours.clear();//清空所有数据
	hierarchy.clear();
	//操作会影响binary_image，必须全部重新填充，并且需要补上边缘
	findContours(binary_image, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
	//填充线虫区域内部的空白杂点
	for (int i = 0; i >= 0; i = hierarchy[i][0]){
		double hole_area = contourArea(contours[i]);
		bool retain_hole = hole_area > this->area * BW::MINIMUM_HOLE_PROPORTION;
		if (!retain_hole)
			this->area += hole_area;
		drawContours(binary_image, contours, i, 255 * retain_hole, CV_FILLED, 8, hierarchy);
	}
	binary_image = ~binary_image;
	//修补图像边缘
	for (int i = 0; i < binary_image.rows; ++i){
		uchar * Mi = binary_image.ptr<uchar>(i);
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
//public:

void Candidate_Points_Detect::Save2File(string dist_cache_dir, string lap_cache_dir, string pic_num) const{
	Save_Mat_To_File<float>(distance_matrix, dist_cache_dir + pic_num);
	ofstream file((lap_cache_dir + pic_num).c_str(), ios::binary);
	for (int i = 0;i < distance_matrix.rows;++ i)
		file.write(reinterpret_cast<char *>(laplacian_matrix[i]), distance_matrix.cols * sizeof(double));
	file.close();
}

double Candidate_Points_Detect::Get_Dist(double x, double y) const{
	x -= Worm_XY_Range[2];
	y -= Worm_XY_Range[0];
	if (x<0 || y<0 || x>=distance_matrix.rows || y>= distance_matrix.cols)
		return 0;
	double alpha, beta;
	int x0 = int(x),y0 = int(y);
	alpha = x - x0;
	beta = y - y0;
	return (distance_matrix.at<float>(x, y) * (1-alpha) * (1-beta) + 
		distance_matrix.at<float>(x+1, y) * alpha * (1-beta) +
		distance_matrix.at<float>(x, y+1) * (1-alpha) * beta +
		distance_matrix.at<float>(x+1, y+1) * alpha * beta);
}

void Candidate_Points_Detect::Detect_Points(const Mat & image, Candidate_Points & candidate_points, double width, double area){
	candidate_points.Reset();
	binary_image = image < BW::BINARY_THRESHOLD;
	Denoise_And_Worm_Locate(area);
	//Save_Mat_To_File<uchar>(image, CACHE_DIR+"binary_denoised\\"+pic_num_str);
	distanceTransform(binary_image, distance_matrix, CV_DIST_L2, CV_DIST_MASK_PRECISE);
	Distance_Retrace(width);
	Calc_LapMat_Of_Inner_Part();
	Catch_Candidate_By_LapMat(candidate_points);
}