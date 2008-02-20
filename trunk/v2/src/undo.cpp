/*
 Voxelbrain Project. MRI editing software.
 This file is (c) 2007,2008
 Nanyang Technological University

 author: Konstantin Levinski
 description:


*/
#include "undo.h"
using namespace std;

void undo::clear(){
	cur_step.clear();
	the_undo.clear();
};

void undo::add_point(size_t to_insert){
	cur_step.push_back(to_insert);
};
void undo::save(){
	the_undo.push_back(cur_step);
	cur_step.clear();
};

void undo::restore(undo::undo_step & out){
	cur_step.clear();
	if(the_undo.size()==0)return; //no undo info
	undo_step cur(the_undo.back()); //get list pointer
	
	for(undo_step::iterator i = cur.begin(); i != cur.end(); i++){
		out.push_back(*i);
	};
	
	the_undo.pop_back();
	
};

bool undo::empty(){
	return (the_undo.size()==0);
};


