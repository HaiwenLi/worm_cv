#include "stdafx.h"

using namespace std;

//private:
void Candidate_Points::Range_Calc(const vector<int> & base_points) const{
	range_x[0] = WORM::IMAGE_SIZE;
	range_x[1] = 0;
	range_y[0] = WORM::IMAGE_SIZE;
	range_y[1] = 0;
	for (auto itor = base_points.begin();itor != base_points.end();++ itor){
		if (cood[*itor][0] < range_x[0])
			range_x[0] = cood[*itor][0];
		if (cood[*itor][0] > range_x[1])
			range_x[1] = cood[*itor][0];
		if (cood[*itor][1] < range_y[0])
			range_y[0] = cood[*itor][1];
		if (cood[*itor][1] > range_y[1])
			range_y[1] = cood[*itor][1];
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

int Candidate_Points::Branch_Analize(vector<int> & point_find, int * branch_stamp) const{
	if (point_find.empty())
		throw new Simple_Exception("Branch_Analize:Input Should Include At Least 1 Point!");
	if (point_find.size() == 1)
		return 1;
	if (point_find.size() == 2)
		if (Is_8_Neighbour(cood[point_find[0]], cood[point_find[1]]))
			return 1;
		else {
			branch_stamp[0] = 0;
			branch_stamp[1] = 1;
			return 2;
		}

	int i, j, k, branch_min, branch_max, branch_num = point_find.size();
	for (i = 0;i < point_find.size();++ i)
		branch_stamp[i] = i;
	for (i = 0;i < point_find.size();++ i)
		for (j = i+1;j< point_find.size();++ j){
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
			for (k = branch_min;k < point_find.size();++ k)
				if (branch_stamp[k] == branch_max)
					branch_stamp[k] = branch_min;
				else if (branch_stamp[k] > branch_max)
					-- branch_stamp[k];
			-- branch_num;
		}
	return branch_num;
}

const double *Candidate_Points::Get_Center(const vector<int> & points) const{
	if (points.empty())
		throw new Simple_Exception("Get_Center:Input Should Include At Least 1 Point!");
	static double center[2];
	center[0] = 0;
	center[1] = 0;
	for (auto itor = points.begin();itor < points.end();++ itor){
		center[0] += cood[*itor][0];
		center[1] += cood[*itor][1];
	}
	center[0] /= points.size();
	center[1] /= points.size();
	return center;
}

void Candidate_Points::Query_Points_Nearby(const vector<int> & base_points, vector<int> & nearby_points) const{
	if (base_points.empty())
		throw new Simple_Exception("Query_Points_Nearby:Input Should Include At Least 1 Point!");

	nearby_points.clear();
	Range_Calc(base_points);
	auto start_index = hash_table[range_x[0]];
	auto end_index = hash_table[range_x[1] + 1];

	for (auto i = start_index;i < end_index;++ i){
		// 由于y坐标已经超出，由candidate_points的有序性，可以直接从hash表中读取x坐标+1后的下标值
		if (cood[i][1] > range_y[1]){
			i = hash_table[cood[i][0] + 1] - 1;
			continue;
		}
		for (auto j = base_points.begin();j != base_points.end();++ j)
			if (Is_8_Neighbour(cood[*j], cood[i])){
				nearby_points.push_back(i);
				break;
			}
	}
}

int Candidate_Points::Query_Points_By_Pointer(const double * base_point, const double * direct_vec) const
{
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
	return metrics_min.Get_Min_Index();

}

void Candidate_Points::persistence(void *obj_ptr, string out_file) {
	ofstream file(out_file.c_str(), ios::binary);
	file.write(reinterpret_cast<char *>(obj_ptr), sizeof(Candidate_Points));
	file.close();
}

void Candidate_Points::anti_persistence(void* obj_ptr, string in_file){
	ifstream file(in_file.c_str(), ios::binary);
	file.read(reinterpret_cast<char *>(obj_ptr), sizeof(Candidate_Points));
	file.close();
}

string Candidate_Points::getPointStr(const vector<int> & points) const{
	stringstream stream;
	string s;
	for (auto i = points.begin();i != points.end();++ i)
		stream << cood[*i][0]<<" "<<cood[*i][1]<<"   ";
	s = stream.str();
	return s;
}

string Candidate_Points::getWholeStr() const{
	stringstream stream;
	string s;
	for (auto i = 0;i < point_num;++ i)
		stream << cood[i][0]<<" "<<cood[i][1]<<"   ";
	s=stream.str();
	return s;
}