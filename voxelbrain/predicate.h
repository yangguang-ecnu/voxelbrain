#ifndef PREDICATE_H_
#define PREDICATE_H_

/*
 * For each cell a predicate provides a list of possible moves;
 * Each of the moves has probability of [0..1]
 * */

#include "v3sets.h"
#include "volio.h"
#include <vector>

using namespace std;

class Propagator {
public:
	class Step:public V3i {
	public:
		Step(const V3i & start, int where):V3i(start), dir_n(where){};
		static V3i directions[];
		static const int DIR_NUM;
		int dir_n;
		void go(V3i & res) const; // step n times
		void go(V3i & res, int steps) const; // step n times
	    float P;  // probability
	};
    Propagator(raw_volume & r):vol(&r){};
	
	raw_volume * vol;
	point_list selected;
	point_list newly_selected; //subset of selected
	
	void apply(const vector<Step> &);
	inline bool is_inside(const V3i &);
	inline Step & rate(Step &);
	void all_moves(const point_list & from, vector<Step> &); //find all the possible steps from the points 
};

#endif /*PREDICATE_H_*/
