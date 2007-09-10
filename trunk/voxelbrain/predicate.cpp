#include "predicate.h"
/*
 * An implementation of a propagation toolkit */

//which directions to try
V3i Propagator::Step::directions[] = {V3i(1,0,0), 
		                       V3i(0,1,0), 
		                       V3i(0,0,1), 
		                       V3i(-1,0,0), 
		                       V3i(1,-1,0), 
		                       V3i(0,0,-1)};

//how many directions in there TODO:try traits/iterators
const int Propagator::Step::DIR_NUM=6;

//TODO: error checking
void Propagator::Step::go(V3i & res) const{
   res.set(x,y,z);
   res+=directions[this->dir_n];
};

void Propagator::Step::go(V3i & res, int steps) const {
   res.set(x,y,z);
   res+=(directions[this->dir_n]*steps);
};

//belongs to the shape
bool Propagator::is_inside(const V3i & to_try){
	return ((*vol)(to_try) > 0 &&
			selected.find(key(to_try)) == selected.end()); 
}

//
void Propagator::all_moves(const point_list & from, vector<Step> & res){
	   V3i to_try; //intermediate point
   for(point_list::iterator i = from.begin(); i!= from.end();i++){
	   for(int where = 0; where < Step::DIR_NUM; where++){
		   Step check(key(*i), where);
		   check.go(to_try);
		   //here we are going to see if it is possible to move there
		   if(
				  is_inside(to_try)  // the shape actually exists there
		     )
		   { 
			   res.push_back(rate(check));
		   }
	   }
   }
}

Propagator::Step & Propagator::rate(Propagator::Step & in){
	V3i tmp;
	float orig = (*vol)(in);
	in.go(tmp); float dest = (*vol)(tmp);
	float diff = (orig>dest)?orig-dest:dest-orig;
	in.P = 1.0-0.001*diff; //difference
	
	//if this direction leads outside; get rid of those caps
	for(int i = 2; i < 4; i++){
		in.go(tmp, i); if(!is_inside(tmp))in.P=1.0; //then go in this direction
	}
	return in;
};

void Propagator::apply(const vector<Propagator::Step> & steps_list){
	typedef vector<Propagator::Step> t_steps_list;
	V3i tmp;
	for(t_steps_list::const_iterator i = steps_list.begin(); i!= steps_list.end(); i++){
	   i->go(tmp);
	   newly_selected.insert(key(tmp)); // mark them as inserted.	
	   selected.insert(key(tmp));	
	};
};

