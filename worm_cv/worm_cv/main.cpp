#include "stdafx.h"
//#define __START_FROM_PART_0

using namespace std;
using namespace cv;

//===================================================函数列表==================================================
void Image_Get(Mat & binary_image, int img_index);
void Denoise_And_Worm_Locate(Mat & binary_image);
void Candidate_Center_Points_Detect(Mat & binary_image, Mat & distance_matrix);

Worm_Data worm_data;

clock_t _t[PARTS], __t, __t_ = 0;
long t[PARTS] = {};
int cparts;

int main(){
	Mat distance_matrix, binary_image;
	const int PIC_NUM = 1;
	clock_t _start = clock();
	for (worm_data.pic_num = 0;worm_data.pic_num < PIC_NUM;++ worm_data.pic_num){
		cparts = 0;
		_t[cparts] = clock();
#ifdef __START_FROM_PART_0
		ifstream f1("..\\..\\data\\beng_data", ios::binary);
		f1.read((char *)worm_data.worm_area_size, 2*sizeof(int));
		f1.read((char *)&worm_data.candidate_center_points, sizeof(worm_data.candidate_center_points));
		f1.close();
		FileStorage fs("..\\..\\data\\beng_dist_data", FileStorage::READ);
		fs["dist"] >> distance_matrix;
#else
		Image_Get(binary_image, worm_data.pic_num);
		Time_Calc();
		Denoise_And_Worm_Locate(binary_image);
		Candidate_Center_Points_Detect(binary_image, distance_matrix);
#endif
		Skeletonize().Convert_To_Graph();
		Root_Search().Search_Backbone();
		Root_Smooth().Interpolate_And_Equal_Divide(worm_data.backbone);
		// 后期数据处理，包括虫体宽度计算、时针方向计算等等，下一幅图片计算时需要依赖此方法的计算结果
		worm_data.Data_Processing(distance_matrix);
		t[cparts] += clock() - _t[cparts];
	}

	long tall = clock()-_start;
	cout<<"total time:\t"<<tall<<"ms"<<endl;
	cout<<"exclude read:\t"<<tall - t[0]<<"ms"<<endl;
	cout<<"time per pic:\t"<<(clock()-_start)/double(PIC_NUM)<<endl;
	cout<<"exclude read:\t"<<(tall - t[0])/(double(PIC_NUM))<<endl;
	for (int i = 0; i <= cparts; i++){
		cout<<"Part "<<i+1<<":  "<<t[i]<<"ms\t\tPer pic:"<<t[i]/double(PIC_NUM)<<endl;
		cout<<"Percentage:"<<setprecision(3)<< 100.0*t[i]/tall<<"\n";
	}
	system("pause");
	return 0;
}

void Image_Get(Mat & binary_image, int img_index){
	// 读取图片，获取其大小并作二值化
	/*string image_filename = "..\\..\\worm data 20140121\\MultiImage_video1_"
							+ num2str(worm_data.pic_num + 1001) + ".tiff";*/
	string image_filename = "..\\..\\beng_fig.tiff";
	binary_image = imread(image_filename, 0);
	if (!binary_image.data){
		cout << "Could not open or find the image" << endl;
		system("pause");
		exit(-1);
	}
	if (binary_image.rows != WORM::IMAGE_SIZE || binary_image.cols != WORM::IMAGE_SIZE){
		cout<<"Image Size Not Fit!"<<endl;
		system("pause");
		exit(-1);
	}
}