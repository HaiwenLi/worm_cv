#include "stdafx.h"
#include <sstream>
#include <string>
#include <iostream>

using namespace std;
using namespace ROOT_SEARCH;
using namespace ROOT_SMOOTH;

void Worm_Data::Data_Processing(const Candidate_Points_Detect & detect){
	for (int i = 0;i <= PARTITION_NUM;++ i){
		worm_width[i] = detect.Get_Dist(backbone.cood[i][0], backbone.cood[i][1]);
	}
	worm_area = detect.Get_Area();
	sort(worm_width, worm_width + PARTITION_NUM);
	worm_full_width = 2 * worm_width[int(PARTITION_NUM * 0.8)];
	clockwise_head = backbone.Clockwise_Direct_Calc(0, PARTITION_NUM / 2);
	clockwise_tail = backbone.Clockwise_Direct_Calc(PARTITION_NUM / 2, PARTITION_NUM - 1);
	clockwise_whole = backbone.Clockwise_Direct_Calc(0, PARTITION_NUM - 1);
	is_first_pic = false;
}
