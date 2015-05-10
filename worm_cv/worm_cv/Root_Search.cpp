#include "stdafx.h"

using namespace std;
using namespace ROOT_SEARCH;

Root_In_Graph::Root_In_Graph(const Root_In_Graph & root):length(root.length), circle(root.circle),
	size(root.size), adhesion_index(root.adhesion_index){
	index = new int[size];
	for (int i= 0; i < length; i++)
		index[i] = root.index[i]; 
}
Root_In_Graph & Root_In_Graph::operator= (const Root_In_Graph & root){
	length = (root.length);
	circle = root.circle;
	if (size < length){
		size = length;
		delete[] index;
		index = new int[length];
	}
	for (int i= 0; i < length; i++)
		index[i] = root.index[i]; 
	return * this;
}
/*
	Obtain_Coodinate
	函数说明：获取路径root_in_graph中节点的中心坐标，并存入centerline中
	参数说明：
	root_in_graph：Root_In_Graph类型的引用，表示路径
	centerline：Centerline类型的引用，表示线虫中心线
	返回值：无
*/
void Root_Search::Obtain_Coodinate(const Root_In_Graph & root_in_graph, Centerline & centerline){
	int length = root_in_graph.length;
	centerline.length = length;
	centerline.circle = root_in_graph.circle;
	if (centerline.circle != NONE)
		centerline.adhesion_index = root_in_graph.adhesion_index;
	for (int i = 0;i < length;++ i){
		centerline.cood[i][0] = graph.Get_Node(root_in_graph[i]).center[0];
		centerline.cood[i][1] = graph.Get_Node(root_in_graph[i]).center[1];
	}
}
/*
	Delete_Root
	函数说明：删除下标为n的路径
	参数说明：
	n：int类型，表示要删除的路径下标
	返回值：无
*/
void Root_Search::Delete_Root(int n){
	if (n < 0 || n >= root_num)
		return;
	delete centerline_candidates[n];
	-- root_num;
	centerline_candidates[n] = centerline_candidates[root_num];
	centerline_candidates[root_num] = NULL;
}
/*
	Short_Root_Filter
	函数说明：删除短路径
	参数说明：
	circle_mode：bool类型，表示是否存在圈
	返回值：无
*/
void Root_Search::Short_Root_Filter(bool circle_mode){
	int minimum_length = max_length;
	if (circle_mode)
		minimum_length -= ROOT_SEARCH::ROOT_SCALE_ERROR;
	for (int i = 0; i < root_num; i++)
		if (centerline_candidates[i] -> length < minimum_length){
			Delete_Root(i);
			-- i;
		}
}
/*
	Match_Correct_Head
	函数说明：判断线虫头尾
	参数说明：无
	返回值：无
*/
void Root_Search::Match_Correct_Head(){
	//const double * head_node, * tail_node;
	static double head[2];//记录中心线头部区域点
	static double tail[2];//记录中心线尾部区域点
	int length;
	if (worm_data.is_first_pic)
		return;
	for (int i = 0; i < root_num; i++){
		//使用距离的方法判断线虫头尾时可考虑头部和尾部区域的所有点
		/*
		head_node = centerline_candidates[i] -> coodinate[0];
		tail_node = centerline_candidates[i] -> coodinate[centerline_candidates[i] -> length - 1];

		dist = Point_Dist_Square(head_node, worm_data.backbone.coodinate[0]) + 
			Point_Dist_Square(tail_node, worm_data.backbone.coodinate[ROOT_SMOOTH::PARTITION_NUM]);
		dist_inv = Point_Dist_Square(head_node, worm_data.backbone.coodinate[ROOT_SMOOTH::PARTITION_NUM]) + 
			Point_Dist_Square(tail_node, worm_data.backbone.coodinate[0]);
		*/
		length = centerline_candidates[i] -> length;//该条中心线的长度

		head[0] = centerline_candidates[i] -> cood[0][0] ;
		head[1] = centerline_candidates[i] -> cood[0][1];
		tail[0] = centerline_candidates[i] -> cood[length-1][0];
		tail[1] = centerline_candidates[i] -> cood[length-1][1];

		double dist = Point_Dist_Square(head, worm_data.backbone.cood[0]) + 
			Point_Dist_Square(tail, worm_data.backbone.cood[ROOT_SMOOTH::PARTITION_NUM ]);

		double dist_inv = Point_Dist_Square(head, worm_data.backbone.cood[ROOT_SMOOTH::PARTITION_NUM]) + 
			Point_Dist_Square(tail, worm_data.backbone.cood[0]);
		// 判断是否需要头尾反向
		if (dist_inv - dist > ROOT_SEARCH::HEAD_CHOOSE_DIST_MIN)
			continue;
		if (dist - dist_inv > ROOT_SEARCH::HEAD_CHOOSE_DIST_MIN){
			centerline_candidates[i] -> Reverse();
			continue;
		}
		//计算时针方向（与上一帧对比）
		if (centerline_candidates[i] -> Clockwise_Direct_Calc(0, centerline_candidates[i] -> length - 1) == worm_data.clockwise_whole)
			continue;
		if (centerline_candidates[i] -> Clockwise_Direct_Calc(0, centerline_candidates[i] -> length - 1) + worm_data.clockwise_whole == 3){
			centerline_candidates[i] -> Reverse();
			continue;
		}
		throw new Simple_Exception("Head Choose Uncertain");
	}
}
/*
	Root_Add
	函数说明：将new_root表示的路径加入到中心线序列(centerline_candidates)
	参数说明：
	new_root_in_graph：Root_In_Graph类型的引用，表示路径
	返回值：无
*/
void Root_Search::Root_Add(const Root_In_Graph & new_root_in_graph){
	// 若栈满则报错
	if (root_num >= SKELETONIZE::STORAGE_MAX)
		throw new Simple_Exception("Root_Search:Root Num Exceed Error!");
	// 将图中的路径转化为坐标路径
	Centerline * new_root = new Centerline(new_root_in_graph.length);
	Obtain_Coodinate(new_root_in_graph, * new_root);
	if (new_root -> circle == TAIL)
		new_root -> Tail_Cut();
	// circle_mode当且仅当最长路径与当前路径都为环路时才为真，这时可以保留多条较长的路径，否则只保留最长的一条
	bool circle_mode = !longest_root_not_circle && (new_root_in_graph.circle != NONE);
	int minimum_length = max_length;
	if (circle_mode)
		minimum_length -= ROOT_SEARCH::ROOT_SCALE_ERROR;
	// 判断路径长度是否达到添加的下限
	if (new_root_in_graph.length <= minimum_length){
		delete new_root;
		return;
	}
	// 若最大长度变长则筛选剩下的路径
	if (new_root_in_graph.length > max_length){
		max_length = new_root_in_graph.length;
		Short_Root_Filter(circle_mode);
		longest_root_not_circle = (new_root_in_graph.circle == NONE);
	}
	// 若不成环则直接添加，否则判断是否与之前的路径首尾结点重复
	if (longest_root_not_circle)
		centerline_candidates[root_num ++] = new_root;
	else{
		int root_with_same_endpoints = -1;
		for (int i = 0; i < root_num; i++)
			if (new_root ->Have_Same_Endpoints(* centerline_candidates[i]))
				root_with_same_endpoints = i;
		// 若无重复则直接添加
		if (root_with_same_endpoints < 0)
			centerline_candidates[root_num ++] = new_root;
		// 若发生重复则保留较长的一条
		else if (new_root -> length > centerline_candidates[root_with_same_endpoints] -> length){
			delete centerline_candidates[root_with_same_endpoints];
			centerline_candidates[root_with_same_endpoints] = new_root;
		}
		else
			delete new_root;
	}
}
/*
	Root_Select
	函数说明：从中心线序列（centerline_candidates）中选择合适的路径作为线虫的中心线
	参数说明：无
	返回值：无
*/
void Root_Search::Root_Select(){
	Clockwise_Direct clockwise_1, clockwise_2;
	switch (root_num){
	case (0):
		throw new Simple_Exception("Root_Search:No Root Found!");
	case (1):
		return;
	case (2):{
		Centerline * root_1 = centerline_candidates[0];
		Centerline * root_2 = centerline_candidates[1];
		if (root_1 -> circle != root_2 -> circle)
			throw new Simple_Exception("Root_Search:Two Circle Roots Must Have Same Head!");
		int adhesion_1 = root_1 -> adhesion_index;
		int adhesion_2 = root_2 -> adhesion_index;
		bool cannot_select = false;
		if (root_1 -> circle == HEAD){
			clockwise_1 = root_1 -> Clockwise_Direct_Calc(0, adhesion_1 - MINIMUM_HALF_CIRCLE);
			clockwise_2 = root_2 -> Clockwise_Direct_Calc(0, adhesion_2 - MINIMUM_HALF_CIRCLE);
			if (clockwise_1 == clockwise_2)
				cannot_select = true;
			if (clockwise_2 == worm_data.clockwise_head)
				swap(centerline_candidates[0], centerline_candidates[1]);
		}
		else{ //if (root_1 -> circle == TAIL)
			clockwise_1 = root_1 -> Clockwise_Direct_Calc(adhesion_1 + MINIMUM_HALF_CIRCLE, root_1 -> length - 1);
			clockwise_2 = root_2 -> Clockwise_Direct_Calc(adhesion_2 + MINIMUM_HALF_CIRCLE, root_2 -> length - 1);
			if (clockwise_1 == clockwise_2)
				cannot_select = true;
			if (clockwise_2 == worm_data.clockwise_tail)
				swap(centerline_candidates[0], centerline_candidates[1]);
		}
		if (cannot_select)
			throw new Simple_Exception("Root_Search: Cannot Choose Root Through Clockwise!");
		return;
	}
	default:
		throw new Simple_Exception("Root_Search:Root Number Larger Than 2!");
	}
}
// Root_Search:public
Root_Search::Root_Search():max_length(0), root_num(0), graph(worm_data.graph){
	for (int i = 0; i < SKELETONIZE::STORAGE_MAX; i++)
		centerline_candidates[i] = NULL;
}
//从骨架图中搜索线虫中心线
void Root_Search::Search_Backbone(){
	Graph_Node next_node;
	int end_node_num, *end_node, current_index, root_stack_top = 0;
	int * node_in_root = new int[graph.Get_Node_Num()];
	Root_In_Graph * root_stack[SKELETONIZE::STORAGE_MAX];
	Root_In_Graph current_root(graph.Get_Node_Num());

	end_node = graph.Get_End_Node();//获取骨架图中所有端点
	end_node_num = graph.Get_End_Num();
	if (end_node_num == 0)
		throw new Simple_Exception("Circle Error!");

	for (int num = 0;num < end_node_num;++ num){//从每个端点出遍历
		for (int i = 0;i < graph.Get_Node_Num();++ i)
			node_in_root[i] = -1; 
		current_index = end_node[num];
		current_root.length = 0;
		current_root.circle = NONE;
		while (true){
			current_root.Add_Node(current_index);//将当前节点加入到路径中
			node_in_root[current_index] = current_root.length - 1;

			const Graph_Node & current_node = graph.Get_Node(current_index);
			current_node.Find_Available_Adjacents(next_node, node_in_root);
			switch (next_node.degree){
				case (1):
					current_index = next_node[0];
					break;
				case (0):
					// 存储路径
					if (current_root.length > max_length - ROOT_SEARCH::ROOT_SCALE_ERROR){
						// 若路径出现环路现象，则找到粘连结点的编号
						if (current_node.degree > 1){
							int adhesion_node = current_node.adjacent[0];
							if (current_root[current_root.length - 2] == adhesion_node)
								adhesion_node = current_node.adjacent[1];
							// 忽略特别短的末端环路，只在环路长度足够时进行处理
							if (node_in_root[adhesion_node] < current_root.length - ROOT_SCALE_ERROR){
								current_root.adhesion_index = node_in_root[adhesion_node];
								current_root.circle = TAIL;
							}
						}
						Root_Add(current_root);
					}
					//读出栈顶路径
 					if (root_stack_top < 1)
						goto END_OF_SEARCH;
					current_root = * root_stack[-- root_stack_top];
					delete root_stack[root_stack_top];
					//初始化
					for (int i = 0;i < graph.Get_Node_Num();++ i)
						node_in_root[i] = -1;
					for (int i = 0;i < current_root.length;++ i)
						node_in_root[current_root[i]] = i;
					current_index = current_root[current_root.length - 1];
					-- current_root.length;
					break;
				default:
					//将第一条路径之外的路径入栈
					for (int i = 1;i < next_node.degree;++ i){
						if (root_stack_top == SKELETONIZE::STORAGE_MAX)
							throw new Simple_Exception("Root Search:Root Stack Num Exceed Error!");
						root_stack[root_stack_top] = new Root_In_Graph(current_root);
						root_stack[root_stack_top] -> Add_Node(next_node[i]);
						++ root_stack_top;
					}
					current_index = next_node[0];
			}
		}
END_OF_SEARCH:;
	}
	delete[] node_in_root;
	Match_Correct_Head();
	Root_Select();
	//cout<<"Root selected, length: "<<worm_data.backbone.length<<endl;
	worm_data.backbone = * centerline_candidates[0];
	for (int i = 0; i < root_num; i++)
		delete centerline_candidates[i];
}
