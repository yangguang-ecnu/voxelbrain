#include "misc.h"

float smooth_bell(float x){
	if(x<0.0f)x=-x;
	if(x>1.0f)return 0.0f;
	float square_x = x*x;
	return 2*square_x*x-3*square_x+1.0f;
};
/// Determines if the state of the object is valid.
/// Rationale - check if a sequence of operations succeeded.
 Validatable::Validatable(){valid_ = true;};
 bool Validatable::valid(){return valid_;};
  bool Validatable::valid(bool is_valid){valid_ = is_valid; return valid_;};
Validatable::operator bool(){return valid_;};

