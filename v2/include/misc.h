/*
 Voxelbrain Project. MRI editing software.
 This file is (c) 2007,2008
 Nanyang Technological University

 author: Konstantin Levinski
 description:


*/
#ifndef MISC_H_
#define MISC_H_

// functions that don't depend on anything and can be useful in many places

#include <string>


float smooth_bell(float x); // f(-1)=0; f(0)=1; f(1)=0.


#define ABS( X) (((X)>0)?(X):(-(X)))

#endif /*MISC_H_*/
