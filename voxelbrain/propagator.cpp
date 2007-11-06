#include "propagator.h"
#include "misc.h"
#include <algorithm>

using namespace std;

//#define ABS(A) (((A)>0)?(A):-(A))


void propagator_t::plan(const raw_volume & vol){
  proposed.clear(); // remove previous planning
  
  for(point_list::iterator c = active.begin(); c!=active.end(); c++){
    for(int i = -1; i<=1; i++)//loop around single point
      for(int j = -1; j<=1; j++)
	for(int k = -1; k<=1; k++){
	  V3i dir_cur(i,j,k); 
	  step step_cur;
	  step_cur.start = key(*c);
	  step_cur.to = dir_cur;
	  step_cur.P = eval(step_cur, vol);
	  if(step_cur.P > 0){ // if it makes sense at all
	    proposed.push_back(step_cur);
	  };
	};	    	
  };
};
	
void propagator_t::set_band(const raw_volume & vol) {
	//analyzing stuff:
	int min = 10000;
	int max = 0;
	for (point_list::iterator c = active.begin(); c!=active.end(); c++) {
		int cur = vol(key(*c));
		if (cur > max)
			max = cur;
		if (cur < min)
			min=cur;
	};

	half_band_size = (float)(max-min)/2.0f;
	//add margins
	half_band_size = half_band_size*1.2;
	band_center = (float)(max+min)/2;
	// go around same points and plan the move
}
	
float propagator_t::eval(const step & s, const raw_volume & vol){
  ///check if it is valid at all:
  V3i dest(s.start+s.to);
  if(active.find(key(dest)) != active.end())return -1.0f;// 1:does not hit active area;
  if(vol(dest)<=0)return -1.0f; // 2:does not end up in outer space
  if(vol(s.start)<=0)return -1.0f; // 3:does not _START_ in outer space; something is wrong;
  float dist=0.0f;
  V3f tmp((float)s.to.x, (float)s.to.y, (float)s.to.z);
  float delta = ABS((float)(vol(dest)-vol(s.start))/(tmp.length()));

  //taking into account band (so that we don't shift gradually to unwanted densities)
  float in_band = smooth_bell((vol(dest)-band_center)/half_band_size);
  
  // check friends:
  int friends = 0;
  for(int i =-1; i <=1; i++)
  	  for(int j =-1; j <=1; j++)
  		  for(int k =-1; k <=1; k++){
  			  if(active.find(key(V3i(dest.x+i, dest.y+j, dest.z+k)))!=active.end())friends++;
  };
  
  //check if we can escape:
  for(int i = 1; i < 3; i++){
    V3i future_dest(s.to*i+s.start);
    if((vol(future_dest)<=0) ||
       (active.find(key(future_dest)) != active.end()))return 1.0f; //if we can reach out, go for it.
  };
		
  return (1.0f-delta/1000.0f)*in_band*(float)friends/27.0; //so far just delta
}
	
	
	
float propagator_t::act(const raw_volume & vol){  //apply the selected steps; //do not alter anything
  if(proposed.size() < 1)return -1; //nothing to do
  sort(proposed.begin(), proposed.end());

  fresh.clear();
  
  int i;
  for(i = 0; i < (proposed.size()/7+1); i++){
    step cur(proposed[i]);
    V3i dest(cur.start+cur.to);
    undo_selection.add_point(key(dest));
    active.insert(key(dest));
    fresh.insert(key(dest));
    poi = dest; //set point of interest
  };
  step last = proposed[i];
  //printf("Edge probability: %f\n", last.P);
  undo_selection.save();
 // update_border(border, active);
 // printf("border size: %d; totl size %d;\n", border.size(), active.size());
  return(last.P);
};

void propagator_t::undo_step(){
	  if(undo_selection.empty())return; //nothing to undo, lah
	  undo::undo_step cur;
	  undo_selection.restore(cur);
	  for(undo::undo_step::iterator i = cur.begin(); i != cur.end(); i++){
		  active.erase(*i);
		  poi = key(*i);
	  };
	  
}

	
	
	
