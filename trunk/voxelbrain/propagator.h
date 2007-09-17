#ifndef PROPAGATOR_H_
#define PROPAGATOR_H_

#include "v3sets.h"
#include "nifti.h"
#include "volio.h"
#include <vector>

using namespace std;

inline int dist(int a, int b){
  return (a>b)?(a-b):(b-a);
}; //TODO: move to misc

//remove surface vor values [from to]

struct propagator_t {

	struct step {
		V3i start;
		V3i to;
		float P;
		bool operator<(const step & other) const {
			return P > other.P;
		}
	};

  typedef vector<step> steps_t;
  steps_t proposed;
  point_list active;
  float min;
  float max;

  void plan(const raw_volume & vol); //fill in proposed steps
  void act(const raw_volume & vol);  //apply the selected steps; //do not alter anything
  float eval(const step &, const raw_volume & vol);
};

#endif /*PROPAGATOR_H_*/
