#include "undo.h"
using namespace std;

void undo::clear(){
	cur_step.clear();
	the_undo.clear();
};

void undo::add_point(const V3i &to_insert){
	cur_step.insert(key(to_insert));
};
void undo::save(){
	undo_step cur;
	for(point_list::iterator i = cur_step.begin(); i!=cur_step.end(); i++){
		cur.push_back(*i);
	}
	cur_step.clear();
	the_undo.push_back(cur);
};

void undo::restore(undo::undo_step & out){
	if(the_undo.size()==0)return; //no undo info
	for(undo_step::iterator i = the_undo.back().begin(); i != the_undo.back().end(); i++){
		out.push_back(*i);
	};
	the_undo.pop_back();
};

bool undo::empty(){
	return (the_undo.size()==0);
}


