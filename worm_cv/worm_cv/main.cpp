#include "stdafx.h"

using namespace std;
using namespace cv;

void Image_Get(Mat & binary_image, int img_index);

int main(){
	Mat image;
	Search_Backbone search_backbone(1001);
	auto PIC_NUM = 100;
	for (auto pic_num = 0; pic_num < PIC_NUM; ++pic_num){
		Image_Get(image, pic_num);
		search_backbone.Search(image);
	}
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
