/*
 Voxelbrain Project. MRI editing software.
 This file is (c) 2007,2008
 Nanyang Technological University

 author: Konstantin Levinski
 description:


*/
#include "v3.h"
#include "stdio.h"

V3i & Vtr::flip(V3i & out, const V3f & in){
  V3f rel(in-zero);
  out.set((int)(rel.x/unit.x), (int)(rel.y/unit.y), (int)(rel.z/unit.z));
  return out;
};
 
V3f & Vtr::flip(V3f & out, const V3i & in){
  out.set(zero.x+unit.x*in.x, zero.y+unit.y*in.y ,zero.z+unit.z*in.z);
  return out;
};

void inspect(const V3f & in){
  	printf("(%f, %f, %f)\n", in.x, in.y, in.z);
};
