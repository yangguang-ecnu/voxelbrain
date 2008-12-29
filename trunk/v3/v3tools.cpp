#include "v3tools.h"
#include <math.h> //for sin/cos
#include <stdio.h> //printf

/*
  Range structure
*/

#define EPSILON 0.0001 //TODO 
const V3f V3_EPSILON(EPSILON, EPSILON, EPSILON);

Range::Range(){ };
Range::Range(const Range & in):min(in.min), max(in.max){ };
Range::Range(const V3f & lower, const V3f & upper):min(lower), max(upper){ };

bool GreaterThanAll(const V3f & a, const V3f & b){
  return (a.x > b.x) && (a.y > b.y) && (a.z > b.z);
};

bool LessThanAll(const V3f & a, const V3f & b){
  return (a.x < b.x) && (a.y < b.y) && (a.z < b.z);
};

bool ContainsPoint(const Range & r, const V3f & pnt){
  return (LessThanAll(r.min, pnt) && GreaterThanAll(r.max, pnt));
};

bool ContainsRange(const Range & r, const Range & in){
  return (LessThanAll(r.min, in.min) && GreaterThanAll(r.max, in.max));
};

bool IntersectsRange(const Range & r, const Range & in){
  return (GreaterThanAll(r.max, in.min) && LessThanAll(r.min, in.max));
};

Range & ExpandRange(Range & r, const V3f & v){
  for(int i = 0; i < 3; i++){
    if(r.min[i] > v[i]) r.min.SetCoord(i, v[i] - EPSILON);
    if(r.max[i] < v[i]) r.max.SetCoord(i, v[i] + EPSILON);
    };
  return r;
};



///stupid. but too much coffee already

V3f rot_x(V3f in, float r){
  V3f res;
  res.x = in.x;
  res.y = cos(r)*in.y-sin(r)*in.z;
  res.z = sin(r)*in.y+cos(r)*in.z;
  return res;
};

V3f rot_y(V3f in, float r){
  V3f res;
  res.y = in.y;
  res.z = cos(r)*in.z-sin(r)*in.x;
  res.x = sin(r)*in.z+cos(r)*in.x;
  return res;
};

V3f rot_z(V3f in, float r){
  V3f res;
  res.z = in.z;
  res.x = cos(r)*in.x-sin(r)*in.y;
  res.y = sin(r)*in.x+cos(r)*in.y;
  return res;
};

//make basis ortonormal again.
void ortoNormalize(V3f & nnx, V3f & nny, V3f & nnz){
  V3f newy; newy.cross(nnz, nnx);
  V3f newz; newz.cross(nnx, newy);
  newy /= newy.length();
  newz /= newz.length();
  nnx /= nnx.length();
  nny = newy;
  nnz = newz;
};

//Find minimal coordinate.
float min(const V3f & in){
  if(in.x < in.y){
    return (in.x < in.z)?in.x:in.z;
  }else{ //in.x > in.y
    return (in.y < in.z)?in.y:in.z;
  };
};

void say(char *msg, const V3f & v){
  printf("%s:%f:%f:%f\n", msg, v.x, v.y, v.z);
};
