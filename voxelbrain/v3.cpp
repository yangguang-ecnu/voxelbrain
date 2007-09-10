#include "v3.h"

float smoothBell(float x){
  if(x<0)x=-x;
  if(x>1)return 0.0f;
  float square_x = x*x;
  return 2*square_x*x-3*square_x+1; // 2*x^3-3*x^2+1, unit bell.
};

V3i & Vtr::flip(V3i & out, const V3f & in){
  V3f rel(in-zero);
  out.set((int)(rel.x/unit.x), (int)(rel.y/unit.y), (int)(rel.z/unit.z));
  return out;
};
 
V3f & Vtr::flip(V3f & out, const V3i & in){
  out.set(zero.x+unit.x*in.x, zero.y+unit.y*in.y ,zero.z+unit.z*in.z);
  return out;
};

