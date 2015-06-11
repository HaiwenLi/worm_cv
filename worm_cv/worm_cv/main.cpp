#include "stdafx.h"

using namespace std;
using namespace cv;

bool Image_Get(Mat & binary_image, int img_index);

int BW::BINARY_THRESHOLD = 80;
int PIC_START = 4560;
int PIC_NUM = 225;
//const string file_path = "..\\..\\worm_pic\\";
const string file_path = "..\\..\\..\\Elegan_Images\\Group6\\Centroid\\image_";
//const string file_path = "..\\..\\..\\Elegan_Images\\Group6\\Roi\\image_";
const string pic_num_prefix = "g6c_";

int main(){
	Mat image;
	Search_Backbone search_backbone;
	for (auto pic_num = 0; pic_num < PIC_NUM; ++pic_num){
		if (Image_Get(image, pic_num))
			search_backbone.Search(image, pic_num_prefix + num2str(pic_num+PIC_START));
	}
	system("pause");
	return 0;
}

//int main() {
//	Root_Smooth root_smooth;
//	Backbone backbone(ROOT_SMOOTH::PARTITION_NUM + 1);
//	Backbone::anti_persistence(&backbone, "..\\..\\360");
//	root_smooth.Interpolate_And_Equal_Divide(backbone, ROOT_SMOOTH::PARTITION_NUM);
//	backbone.persistence(&backbone, "..\\..\\360out");
//}

bool Image_Get(Mat & image, int img_index){
	string image_filename = file_path + num2str(img_index + PIC_START) + ".tiff";
	image = imread(image_filename, 0);
	if (!image.data)
		return false;
	if (image.rows != WORM::IMAGE_SIZE || image.cols != WORM::IMAGE_SIZE)
		throw new Simple_Exception("Image Size Not Fit!");
	return true;
}
