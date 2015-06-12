#pragma once

#include "CONST_PARA.h"

struct Backbone{
	int length;
	int size;
	double (* cood)[2];

	explicit Backbone(int root_max_length):length(0), size(root_max_length){
		cood = new double[size][2];
	}
	Backbone(const Backbone & root);
	Backbone & operator=(const Backbone & root);
	~Backbone(){
		delete[] cood;
	}
	static void persistence(void *obj_ptr, std::string out_file);
	static void anti_persistence(void *obj_ptr, std::string in_file);
};

