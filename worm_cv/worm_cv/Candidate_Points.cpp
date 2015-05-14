#include "stdafx.h"

using namespace std;

//private:
void Candidate_Points::Range_Calc(const Multi_Points & base_points) const{
	range_x[0] = cood[base_points[0]][0];
	range_x[1] = cood[base_points[0]][0];
	range_y[0] = cood[base_points[0]][1];
	range_y[1] = cood[base_points[0]][1];
	for (int i = 1;i < base_points.size;++ i){
		if (cood[base_points[i]][0] < range_x[0])
			range_x[0] = cood[base_points[i]][0];
		else if (cood[base_points[i]][0] > range_x[1])
			range_x[1] = cood[base_points[i]][0];
		if (cood[base_points[i]][1] < range_y[0])
			range_y[0] = cood[base_points[i]][1];
		else if (cood[base_points[i]][1] > range_y[1])
			range_y[1] = cood[base_points[i]][1];
	}
	-- range_x[0];
	-- range_y[0];
	++ range_x[1];
	++ range_y[1];
}

//public:
void Candidate_Points::Add_Line(){
	hash_table[++ current_line] = point_num;
}

void Candidate_Points::Add_Point_To_Line(int y){
	if (point_num + 1 >= SKELETONIZE::POINT_NUM_MAX)
		throw new Simple_Exception("Candidate_Center_Points:Point Num Exceed Error!");
	cood[point_num][0] = current_line;
	cood[point_num][1] = y;
	++ point_num;
}

int Candidate_Points::Branch_Analize(Multi_Points & point_find, int * branch_stamp) const{
	if (point_find.size <= 0)
		throw new Simple_Exception("Branch_Analize:Input Should Include At Least 1 Point!");
	if (point_find.size == 1)
		return 1;
	if (point_find.size == 2)
		if (Is_8_Neighbour(cood[point_find[0]], cood[point_find[1]]))
			return 1;
		else {
			branch_stamp[0] = 0;
			branch_stamp[1] = 1;
			return 2;
		}

	int i, j, k, branch_min, branch_max, branch_num = point_find.size;
	for (i = 0;i < point_find.size;++ i)
		branch_stamp[i] = i;
	for (i = 0;i < point_find.size;++ i)
		for (j = i+1;j< point_find.size;++ j){
			if (branch_stamp[i] == branch_stamp[j])
				continue;
			if (!Is_8_Neighbour(cood[point_find[i]], cood[point_find[j]]))
				continue;
			if (branch_stamp[i] > branch_stamp[j]){
				branch_max = branch_stamp[i];
				branch_min = branch_stamp[j];
			}
			else{
				branch_min = branch_stamp[i];
				branch_max = branch_stamp[j];
			}
			for (k = branch_min;k < point_find.size;++ k)
				if (branch_stamp[k] == branch_max)
					branch_stamp[k] = branch_min;
				else if (branch_stamp[k] > branch_max)
					-- branch_stamp[k];
			-- branch_num;
		}
	return branch_num;
}

const double *Candidate_Points::Get_Center(const Multi_Points & points) const{
	if (points.size <= 0)
		throw new Simple_Exception("Get_Center:Input Should Include At Least 1 Point!");
	static double center[2];
	center[0] = cood[points[0]][0];
	center[1] = cood[points[0]][1];
	for (int i = 1;i < points.size;++ i){
		center[0] += cood[points[i]][0];
		center[1] += cood[points[i]][1];
	}
	center[0] /= points.size;
	center[1] /= points.size;
	return center;
}

Multi_Points Candidate_Points::Query_Points_Nearby(const Multi_Points & base_points) const{
	if (base_points.size <= 0)
		throw new Simple_Exception("Query_Points_Nearby:Input Should Include At Least 1 Point!");
	Multi_Points nearby_points;

	Range_Calc(base_points);
	int start_index = hash_table[range_x[0]];
	int end_index = hash_table[range_x[1] + 1];

	for (int i = start_index;i < end_index;++ i){
		// 由于y坐标已经超出，由candidate_points的有序性，可以直接从hash表中读取x坐标+1后的下标值
		if (cood[i][1] > range_y[1]){
			i = hash_table[cood[i][0] + 1] - 1;
			continue;
		}
		for (int j = 0;j < base_points.size;++ j)
			if (Is_8_Neighbour(cood[base_points[j]], cood[i])){
				nearby_points.Add(i);
				break;
			}
	}
	return nearby_points;
}

Multi_Points Candidate_Points::Query_Points_By_Pointer(const double * base_point, const double * direct_vec) const{
	using namespace SKELETONIZE;

	int start = hash_table[max(int(base_point[0] - METRICS_MAX), 0)];
	int end = hash_table[min(int(base_point[0] + METRICS_MAX) + 1, current_line)];

	double tan_angle_diff, dist, direct_vec_temp[2];
	Select_Minimum metrics_min(METRICS_MAX, -1);
	for (int i = start; i < end; ++i){
		if (cood[i][1] < base_point[1] - METRICS_MAX ||	cood[i][1] > base_point[1] + METRICS_MAX)
			continue;
		direct_vec_temp[0] = cood[i][0] - base_point[0];
		direct_vec_temp[1] = cood[i][1] - base_point[1];
		tan_angle_diff = Included_Angle_Tan(direct_vec, direct_vec_temp);
		if (tan_angle_diff >= 0 && tan_angle_diff < ANGLE_THRESHOLD_NAN_TAN){//为什么角度变化要大于0？与Angle_Tan的定义有关
			dist = sqrt(Point_Dist_Square(base_point, cood[i]));
			metrics_min.Renew(dist * (1 + ALPHA * tan_angle_diff), i);
		}
	}
	if (metrics_min.Get_Min_Index() > 0)
		return Multi_Points(metrics_min.Get_Min_Index());
	return Multi_Points();

}

void Candidate_Points::persistence(int *obj_ptr, string file_name) {
	ofstream file(file_name.c_str(), ios::binary);
	file.write(reinterpret_cast<char *>(obj_ptr), sizeof(Candidate_Points));
	file.close();
}

string Candidate_Points::getPointStr(const Multi_Points & points) const{
	stringstream stream;
	string s;
	for (int i = 0;i < points.size;++ i)
		stream << cood[points[i]][0]<<" "<<cood[points[i]][1]<<"   ";
	s = stream.str();
	return s;
}

string Candidate_Points::getPointStr() const{
	stringstream stream;
	string s;
	for (int i = 0;i < point_num;++ i)
		stream << cood[i][0]<<" "<<cood[i][1]<<"   ";
	s=stream.str();
	return s;
}