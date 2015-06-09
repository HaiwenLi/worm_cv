#pragma once

#include "CONST_PARA.h"

struct Centerline{
private:
	Centerline(){};
public:
	int length;
	int size;
	double (* cood)[2];

	explicit Centerline(int root_max_length):length(0), size(root_max_length){
		cood = new double[size][2];
	}
	Centerline(const Centerline & root);
	Centerline & operator=(const Centerline & root);
	~Centerline(){
		delete[] cood;
	}
	static void persistence(void *obj_ptr, std::string out_file);
	static void anti_persistence(void *obj_ptr, std::string in_file);
};

