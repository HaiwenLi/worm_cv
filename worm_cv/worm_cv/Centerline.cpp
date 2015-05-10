#include "stdafx.h"

using namespace std;
using namespace ROOT_SEARCH;

Centerline::Centerline(const Centerline & root):length(root.length), circle(root.circle),
	size(root.size), adhesion_index(root.adhesion_index){
	cood = new double[size][2];
	for (int i= 0;i < length;++ i){
		cood[i][0] = root.cood[i][0]; 
		cood[i][1] = root.cood[i][1];
	}
}
Centerline & Centerline::operator = (const Centerline & root){
	length = (root.length);
	circle = root.circle;
	if (size < length){
		size = length;
		delete[] cood;
		cood = new double[length][2];
	}
	for (int i= 0; i < length; i++){
		cood[i][0] = root.cood[i][0]; 
		cood[i][1] = root.cood[i][1];
	}
	return * this;
}
void Centerline::Reverse(){
	double swap_temp[2];
	if (circle != NONE)
		circle = Root_Circle(3 - circle);
	for (int i = 0; i < length/2; i++){
		swap_temp[0] = cood[i][0];
		swap_temp[1] = cood[i][1];
		cood[i][0] = cood[length - i - 1][0];
		cood[i][1] = cood[length - i - 1][1];
		cood[length - i - 1][0] = swap_temp[0];
		cood[length - i - 1][1] = swap_temp[1];
	}
	adhesion_index = length - 1 - adhesion_index;
}
// 对于尾部与身体粘连的中心线，去除其尾部点以保证尾部末端到身体的距离大于虫体宽度的一定倍数
void Centerline::Tail_Cut(){
	if (circle != TAIL) return;
	double minimum_dist_square = worm_data.worm_full_width * worm_data.worm_full_width / 10;
	while (Point_Dist_Square(cood[length - 1], cood[adhesion_index]) <= minimum_dist_square)
		-- length;
}

// 计算从head_index节点到tail_index节点的时针方向
Clockwise_Direct Centerline::Clockwise_Direct_Calc(int head_index, int tail_index){
	if (tail_index - head_index < ROOT_SCALE_ERROR)
		throw Simple_Exception("Clockwise_Direct_Calc: Length Must Be Larger Than ");
	if (head_index < 0 || tail_index >= this -> length)
		throw Simple_Exception("Clockwise_Direct_Calc: Invalid Head_Index Or Invalid Length ");
//cout<<"length"<<this -> length<< "\thead:"<<head_index<<"\ttail:"<<tail_index<<endl;
	double benchmark_point[2] = {0,0}, last_angle, current_angle;
	double full_angle_change = 0, temp_angle_change;
	// 计算基准点(首尾连线中点)，以及基准方向(尾结点到首节点的方向)
	for (int i = head_index; i <= tail_index; ++ i){
		benchmark_point[0] += cood[i][0];
		benchmark_point[1] += cood[i][1];
	}
	benchmark_point[0] /= (tail_index - head_index + 1);
	benchmark_point[1] /= (tail_index - head_index + 1);
	last_angle = atan2(cood[head_index][0] - benchmark_point[0],
		                                     cood[head_index][1] - benchmark_point[1]);
	for (int current_index = head_index + MINIMUM_HALF_CIRCLE - 1;
		current_index < tail_index; current_index += MINIMUM_HALF_CIRCLE){
		
			current_angle = atan2(cood[current_index][0] - benchmark_point[0],
				                                cood[current_index][1] - benchmark_point[1]);
			temp_angle_change = current_angle - last_angle;
			// 将角度变化调整到(-pi, pi]区间内
			temp_angle_change += ((temp_angle_change < - WORM::PI) - (temp_angle_change > WORM::PI)) * 2* WORM::PI;
			full_angle_change += temp_angle_change;
			last_angle = current_angle;
	}
	if (full_angle_change > 0) return CLOCKWISE ;//如果角度和大于0，返回顺时针
	else return COUNTER_CLOCKWISE;//否则返回逆时针
}

void Centerline::Save2File(std::string file_name){
	ofstream file(file_name.c_str(), ios::binary);
	file.write((char *)&length, sizeof(int));
	file.write((char *)cood, 2 * length * sizeof(double));
	file.close();
}
