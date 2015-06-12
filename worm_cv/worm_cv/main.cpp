#include "stdafx.h"

using namespace std;
using namespace cv;

bool Image_Get(Mat & binary_image, int img_index);

int BW::BINARY_THRESHOLD = 80;
int PIC_START = 1951;
int PIC_END = 3100;
//const string file_path = "..\\..\\worm_pic\\";
//const string file_path = "..\\..\\..\\Elegan_Images\\Group6\\Centroid\\image_";
//const string file_path = "..\\..\\..\\Elegan_Images\\Group6\\Roi\\image_";
const string file_path = "..\\..\\..\\image2_tiff\\image_";
const string pic_num_prefix = "i2_";

int main() {
	Mat image;
	Search_Backbone search_backbone;
	for (auto pic_num = PIC_START; pic_num < PIC_END; ++pic_num) {
		if (Image_Get(image, pic_num))
			search_backbone.Search(image, pic_num_prefix + int2str(pic_num));
	}
	system("pause");
	return 0;
}

bool Image_Get(Mat & image, int img_index) {
	auto image_filename = file_path + int2str(img_index) + ".tiff";
	image = imread(image_filename, 0);
	if (!image.data || image.rows != WORM::IMAGE_SIZE || image.cols != WORM::IMAGE_SIZE) {
		cout << "Pic "+int2str(img_index)+" : Not Exist or Wrong Image Size!" << endl;
		return false;
	}
	return true;
}
