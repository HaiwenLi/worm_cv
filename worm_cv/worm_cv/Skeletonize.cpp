#include "stdafx.h"

using namespace std;

const static string js_act="{\"action\":\"";
const static string js_para="\",\"para\":\"";
const static string js_end="\"},";

Multi_Points::Multi_Points(const Multi_Points & multi_points):size(multi_points.size){
	for (int i = 0; i < size; ++ i)
		index[i] = multi_points[i];
}

Multi_Points & Multi_Points::operator=(const Multi_Points & multi_points){
	size = multi_points.size;
	for (int i = 0; i < size; ++ i)
		index[i] = multi_points.index[i];
	return *this;
}

ostream & operator<< (ostream & out, Multi_Points multi_points){
	out<<"size:"<<multi_points.size<<endl;
	for (int i = 0;i < multi_points.size;++ i)
		out<<multi_points[i]<<"\t";
	out<<endl;
	return out;
}

void Bifurcate_Stack::Push(const Multi_Points & in_stack_points, int parent_index){
	if (top >= SKELETONIZE::STORAGE_MAX)
		throw new Simple_Exception("Bifurcate_Stack:Stack Full Error!");
	parent_node[top] = parent_index;
	item[top] = in_stack_points;
	++ top;
}

Owner_Mark::Owner_Mark(){
	reset();
}

void Owner_Mark::reset(){
	specific_index = 0;
	this -> point_available = & Owner_Mark::check_All;
	for (int i = 0;i < SKELETONIZE::POINT_NUM_MAX;++ i)
		owner_node[i] = -1;
}

void Owner_Mark::Set_Owner_Node(Multi_Points points, int node_index){
	for (int i = 0;i < points.size;++ i)
		owner_node[points[i]] = node_index;
}

int Owner_Mark::Get_Owner_Node(int point_index) const{
	return owner_node[point_index];
}

void Owner_Mark::Set_Check_Mode(CHECK_MODE check_mode, int specific_index){
	this -> specific_index = specific_index;
	if (check_mode == CHECK_ALL)
		this -> point_available = &Owner_Mark::check_All;
	else if(check_mode == CHECK_NONE)
		this -> point_available = &Owner_Mark::check_None;
	else
		this -> point_available = &Owner_Mark::check_Specific;
}

void Skeletonize::Add_Into_Graph(int parent_node){
	owner_mark.Set_Owner_Node(current_item, skeleton_graph -> Get_Node_Num());
	skeleton_graph -> Add_Node(candidate_points->Get_Center(current_item), parent_node);
#ifndef __SKIP_DEBUG_INFO
	js_out<<js_act<<"addNode"<<js_end;
#endif
}

void Skeletonize::Current_Item_Bifurcate(int parent_node){
	static int branch_stamp[10];
	static Multi_Points temp;
	int branch_num = candidate_points->Branch_Analize(current_item, branch_stamp);
	if (branch_num > 1){
		for (int i = 0;i < branch_num - 1;++ i){
			temp.size = 0;
			for (int j = 0;j < current_item.size;++ j)
				if (branch_stamp[j] == i)
					temp.Add(current_item[j]);
			stack.Push(temp, parent_node);
#ifndef __SKIP_DEBUG_INFO
			js_out<<"{\"continue\":\"true\",\"action\":\"setStack"<<js_para<<candidate_points->getPointStr(temp)<<js_end;
#endif
		}
		int size = current_item.size;

		current_item.size = 0;
		for (int j = 0;j < size;++ j)
			if (branch_stamp[j] == branch_num - 1)
				current_item.Add(current_item[j]);
	}
#ifndef __SKIP_DEBUG_INFO
	js_out<<js_act<<"select"<<js_para<<candidate_points->getPointStr(current_item)<<js_end;
	if (branch_num > 1)
		js_out<<js_act<<"push"<<js_end;
#endif
}

bool Skeletonize::Search_Next_Points(){
	int current_node = skeleton_graph->Get_Node_Num()-1;
	double direction_vec[2];
	current_item = candidate_points -> Query_Points_Nearby(current_item, owner_mark);
	if (current_item.size <= 0 && skeleton_graph -> Calc_End_Direction_Vec(current_node, direction_vec)){
		current_item = candidate_points->Query_Points_By_Pointer
			(skeleton_graph -> Get_Center(current_node), direction_vec, owner_mark);
	}

	if (current_item.size <= 0)
		return false;
	else if (current_item.size == 1){
#ifndef __SKIP_DEBUG_INFO
		js_out<<js_act<<"select"<<js_para<<candidate_points->getPointStr(current_item)<<js_end;
#endif
		current_item = candidate_points -> Query_Points_Nearby(current_item, owner_mark);
#ifndef __SKIP_DEBUG_INFO
		if (current_item.size > 1)
			js_out<<js_act<<"reselect"<<js_para<<candidate_points->getPointStr(current_item)<<js_end;
#endif
	}
	else 
		Current_Item_Bifurcate(current_node);
	Add_Into_Graph(skeleton_graph->Get_Node_Num()-1);
	return true;
}

bool Skeletonize::Check_Up_Stack(){
	bool node_used;
	while (stack.top > 0){
		node_used = false;
		int stack_top = -- stack.top;
#ifndef __SKIP_DEBUG_INFO
		js_out<<js_act<<"pop"<<js_end;
#endif
		for (int i = 0;i < stack.item[stack_top].size;++ i)
			if (!(owner_mark.*owner_mark.point_available)(stack.item[stack_top][i])){
				// 若结点已经使用则将其与母节点相连接
				int zi_node = owner_mark.Get_Owner_Node(stack.item[stack_top][i]);
				skeleton_graph -> Connect_Node(zi_node, stack.parent_node[stack_top]);
#ifndef __SKIP_DEBUG_INFO
				js_out<<"{\"continue\":\"true\",\"action\":\"connectNode"<<js_para<<num2str(zi_node) + " " + num2str(stack.parent_node[stack_top])<<js_end;
				js_out<<js_act<<"unselect"<<js_end;
#endif
				node_used = true;
				break;
			}
		if (node_used == false){// 若结点未使用过则表示栈中已找到所需元素
			current_item = stack.item[stack_top];
#ifndef __SKIP_DEBUG_INFO
			js_out<<js_act<<"setCurrent"<<js_para<<num2str(stack.parent_node[stack_top])<<js_end;;
#endif
			Add_Into_Graph(stack.parent_node[stack_top]);
			return true;
		}
	}
	return false;
}

void Skeletonize::Search_Unused_Points(){
	for (int i = 0; i < point_num;++ i)
		if ((owner_mark.*owner_mark.point_available)(i)){
			current_item = candidate_points->Query_Points_Nearby((Multi_Points) i, owner_mark);
#ifndef __SKIP_DEBUG_INFO
			js_out<<js_act<<"select"<<js_para<<candidate_points->getPointStr(current_item)<<js_end;
#endif
			Add_Into_Graph(-1);
			break;
		}
}

void Skeletonize::Connecting_End(){	
	int end_node_num, * end_nodes;
	double direction_vec[2];
	bool end_node_changed = true;
	end_nodes = skeleton_graph->Get_End_Node();
	end_node_num = skeleton_graph->Get_End_Num();
	owner_mark.Set_Check_Mode(Owner_Mark::CHECK_SPECIFIC);
	
	while (end_node_changed){
		end_node_changed = false;
		for (int i = 0;i < end_node_num;++ i){
			if (!skeleton_graph->Is_End_Node(end_nodes[i]))
				continue;
			owner_mark.set_Specific_index(end_nodes[i]);
			current_item.size = 0;
			if (skeleton_graph -> Calc_End_Direction_Vec(end_nodes[i], direction_vec))
				current_item = candidate_points->Query_Points_By_Pointer(skeleton_graph -> Get_Center(end_nodes[i]), direction_vec, owner_mark);
			if (current_item.size == 1){
				end_node_changed = true;
				skeleton_graph->Connect_Node(end_nodes[i], owner_mark.Get_Owner_Node(current_item[0]));
#ifndef __SKIP_DEBUG_INFO
	js_out<<js_act<<"connectNode"<<js_para<<end_nodes[i]<<" "<<owner_mark.Get_Owner_Node(current_item[0])<<js_end;
#endif
			}
			end_nodes[i] = current_item.size==1 ? owner_mark.Get_Owner_Node(current_item[0]) : -1;
		}
	}
}

//创建骨架图，并对该图进行处理（Graph_Pruning）
void Skeletonize::Convert_To_Graph(const Candidate_Points * candidate_points, Graph * skeleton_graph){
#ifndef __SKIP_DEBUG_INFO
	js_out.open("d:\\jstemp\\json"+num2str(worm_data.pic_num)+".js");
	js_out<<"worm_json=["<<js_act<<"init"<<js_para<<candidate_points->getPointStr()<<js_end;
#endif
	this -> owner_mark.reset();
	this -> candidate_points = candidate_points;
	this -> skeleton_graph = skeleton_graph;
	point_num = candidate_points -> Get_Point_Num();
	skeleton_graph -> Reset();

	current_item = 0;//将中心线候选点的第一个点设置为当前点，开始建立骨架图
#ifndef __SKIP_DEBUG_INFO
	js_out<<js_act<<"select"<<js_para<<candidate_points->getPointStr((Multi_Points)0)<<js_end;
#endif

	stack.top = 0;
	Add_Into_Graph(-1);

	while(current_item.size > 0){
		if (Search_Next_Points())
			continue;
#ifndef __SKIP_DEBUG_INFO
		js_out<<js_act<<"removeCurrent"<<js_end;
#endif
		if (Check_Up_Stack())
			continue;
		Search_Unused_Points();
	}
	Connecting_End();
#ifndef __SKIP_DEBUG_INFO
	js_out<<"]";
	js_out.close();
#endif
}