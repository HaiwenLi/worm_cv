#include "stdafx.h"

using namespace std;

Backbone::Backbone(const Backbone & root):length(root.length), size(root.size){
	cood = new double[size][2];
	for (int i= 0;i < length;++ i){
		cood[i][0] = root.cood[i][0]; 
		cood[i][1] = root.cood[i][1];
	}
}
Backbone & Backbone::operator = (const Backbone & root){
	length = (root.length);
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

void Backbone::persistence(void * obj_ptr, string out_file){
	auto typed_ptr = reinterpret_cast<Backbone *>(obj_ptr);
	ofstream file(out_file.c_str(), ios::binary);
	file.write(reinterpret_cast<char *>(&typed_ptr->length), sizeof(int));
	file.write(reinterpret_cast<char *>(typed_ptr->cood), 2 * typed_ptr->length * sizeof(double));
	file.close();
}

void Backbone::anti_persistence(void* obj_ptr, std::string in_file) {
	auto typed_ptr = reinterpret_cast<Backbone *>(obj_ptr);
	ifstream file(in_file.c_str(), ios::binary);
	file.read(reinterpret_cast<char *>(&typed_ptr->length), sizeof(int));
	typed_ptr->size = typed_ptr->length;
	if (typed_ptr->cood != nullptr)
		delete[] typed_ptr->cood;
	typed_ptr->cood = new double[typed_ptr->length][2];
	file.read(reinterpret_cast<char *>(typed_ptr->cood), 2 * typed_ptr->length * sizeof(double));
	file.close();
}