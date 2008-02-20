/*
 Voxelbrain Project. MRI editing software.
 This file is (c) 2007,2008
 Nanyang Technological University

 author: Konstantin Levinski
 description:


*/
#include "misc.h"

using namespace std;

float smooth_bell(float x){
  if(x<0)x=-x;
  if(x>1)return 0.0f;
  float square_x = x*x;
  return 2*square_x*x-3*square_x+1; // 2*x^3-3*x^2+1, unit bell.
};

