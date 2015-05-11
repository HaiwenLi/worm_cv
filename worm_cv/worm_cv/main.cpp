#include "stdafx.h"

using namespace std;
using namespace cv;

void Image_Get(Mat & binary_image, int img_index);
Centerline *Search_Backbone(Mat image, string pic_num_str = "New Pic");

clock_t _t[PARTS], __t, __t_ = 0;
long t[PARTS] = {};
int cparts;


int main(){
	Mat image;
	clock_t _start = clock();
	int PIC_NUM = 100;
	for (worm_data.pic_num = 0; worm_data.pic_num < PIC_NUM; ++worm_data.pic_num){
		cparts = 0;
		_t[cparts] = clock();
		Image_Get(image, worm_data.pic_num);
		Search_Backbone(image, num2str(worm_data.pic_num + 1001));
		t[cparts] += clock() - _t[cparts];
	}
	long tall = clock() - _start;
	cout << "total time:\t" << tall << "us" << endl;
	cout << "exclude read:\t" << tall - t[0] << "us" << endl;
	cout << "time per pic:\t" << (clock() - _start) / double(PIC_NUM) << endl;
	cout << "exclude read:\t" << (tall - t[0]) / (double(PIC_NUM)) << endl;
	for (int i = 0; i <= cparts; i++){
		cout << "Part " << i + 1 << ":  " << t[i] << "ms\t\tPer pic:" << t[i] / double(PIC_NUM) << endl;
		cout << "Percentage:" << setprecision(3) << 100.0*t[i] / tall << "\n";
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
